#include "BLECallbacks.hpp"
#include "HumiditySensor.hpp"
#include "Plant.hpp"
#include "BLECallbacks.hpp"
#include "CharacteristicCallbacks.hpp"
#include "BLE2902.h"
#include "Wire.h"
#include <Display.hpp>
#include <Util.hpp>
#include "LittleFS.h"

#define SENSOR_READ_DURATION_MS 5000  // Time in MS between sensor readings
#define BLE_NAME "SmartPot"
#define BLE_SERVICE_UUID "6360ec7b-a2b6-41d2-87c6-be45caf92838"
#define BLE_CHARACTERISTIC_UUID "46f45f15-b963-4e4e-bde9-6a9a677df4b4"

// These are the addresses for the sensors 
#define DISPLAY_ADDRESS 0x3C
#define TEMP_ADDRESS 0x40
#define LIGHT_ADDRESS 0x23
#define SOIL_ADDRESS 0x36

Plant plant;
BLEService *pService;
BLECharacteristic *pCharacteristic;
struct ActualConditions results;
String connectionStatus = "Disconnected";

// Method signatures
void bluetoothThreadEntry(void *pvParameters);
void sensorThreadEntry(void *pvParameters);
void bluetoothSetup();
void sensorSetup();
boolean loadPlantDatabaseIntoMemory();

// Global Variables 
JsonDocument plantDatabase;
HumiditySensor hts;
Display display;
BLEServer *pServer;

void setup() {
  Serial.begin(115200);

  boolean status = loadPlantDatabaseIntoMemory();
  if(!status) {
    return;
  }

  plant = Plant(plantDatabase);

  Wire.begin(21, 22);
  Wire.setClock(100000);

  bluetoothSetup();
  sensorSetup();  
  xTaskCreatePinnedToCore(sensorThreadEntry, "Sensor Thread", 8192, NULL, 2, NULL, 1);
}

void loop() {}

// Bluetooth configuration
void bluetoothSetup() {
  // Set name
  BLEDevice::init(BLE_NAME);

  // Create server
  pServer = BLEDevice::createServer();
  // Create Service 
  pService = pServer->createService(BLE_SERVICE_UUID);
  
  // Create Characteristic 
  CharacteristicCallbacks *bleCharCb = new CharacteristicCallbacks();
  pCharacteristic = pService->createCharacteristic(BLE_CHARACTERISTIC_UUID, 
    BLECharacteristic::PROPERTY_READ |
    BLECharacteristic::PROPERTY_NOTIFY
  );
  pCharacteristic->setCallbacks(bleCharCb);

  BLE2902 *p2902Descriptor = new BLE2902();
  pCharacteristic->addDescriptor(p2902Descriptor);

  BLECallbacks *cb = new BLECallbacks(bleCharCb, results, &display, connectionStatus);
  pServer->setCallbacks(cb);


  pService->start();
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(BLE_SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06); 
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
}

// Sensor configuration
void sensorSetup() {
  hts = HumiditySensor(TEMP_ADDRESS, "Humidity and Temperature", "ea825233-6829-4ba3-b907-f6ab8d0a0e9e");
  Serial.printf("Humidity and Temp Sensor initialized: %d", hts.begin());

  
  display = Display(DISPLAY_ADDRESS, "Display", "display-01");
  display.begin();
  display.drawBootScreen();
  delay(5000);
}

// Sensor threading
void sensorThreadEntry(void *pvParameters) {
  JsonDocument dataToTransmit;
 while(1) {
    String data;

    // Read in data from sensor
    hts.readData();

    // Format to JSON for ease of use
    dataToTransmit.add(hts.parseData());

    // Serialize for transmission
    serializeJson(dataToTransmit, data);
    results = Util::formatConditions(dataToTransmit);
  
    display.drawScreen(results, connectionStatus);

    // Only transmit data when clients are connected
    if(pServer->getConnectedCount() > 0) {
      // Set value and notify client of new data
      pCharacteristic->setValue(data.c_str());
      pCharacteristic->notify();
    }
    dataToTransmit.clear();
    // Wait 5 seconds
    vTaskDelay(pdMS_TO_TICKS(SENSOR_READ_DURATION_MS));
  }
}

boolean loadPlantDatabaseIntoMemory() {
  if(!LittleFS.begin(true)) {
    Serial.println("LittleFS Mount failed.");
    return false;
  }

  File file = LittleFS.open("/plantprofiles.json");
  if(!file){
    Serial.println("Failed to open file for reading");
    return false;
  }

  DeserializationError error = deserializeJson(plantDatabase, file);
  file.close();

  if(error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return false;
  }
  return true;
}