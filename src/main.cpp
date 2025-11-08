#include "BLECallbacks.hpp"
#include "HumiditySensor.hpp"
#include <Util.hpp>
#include "Plant.hpp"
#include "BLECallbacks.hpp"
#include "CharacteristicCallbacks.hpp"
#include "BLE2902.h"
#include "Wire.h"
#include "LittleFS.h"
#include <LightSensor.cpp>
#include <Display.hpp>
#include <MoistureSensor.hpp>
#include <Preferences.h>

#define SENSOR_READ_DURATION_MS 5000  // Time in MS between sensor readings
#define BLE_NAME "SmartPot"
#define BLE_SERVICE_UUID "6360ec7b-a2b6-41d2-87c6-be45caf92838"
#define BLE_CHARACTERISTIC_UUID "46f45f15-b963-4e4e-bde9-6a9a677df4b4"

// These are the addresses for the sensors 
#define DISPLAY_ADDRESS 0x3C
#define TEMP_ADDRESS 0x40
#define LIGHT_ADDRESS 0x23
#define SOIL_ADDRESS 0x36

// These are pins
#define RELAY_PIN 5

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
void initPreferenceValues();

// Global Variables 
JsonDocument plantDatabase;
HumiditySensor hts;
LightSensor lightSensor;
MoistureSensor moistureSensor;
Display display;
BLEServer *pServer;
TemperatureScale temperatureScale = FERINEHIGHT;
Preferences preferences;

void setup() {
  Serial.begin(115200);
  pinMode(RELAY_PIN, OUTPUT);
  preferences.begin("prefs", false);

  boolean status = loadPlantDatabaseIntoMemory();
  if(!status) {
    return;
  }

  plant = Plant(&plantDatabase);
  initPreferenceValues();

  Wire.begin(21, 22);
  Wire.setClock(100000);

  bluetoothSetup();
  sensorSetup();  
  xTaskCreatePinnedToCore(sensorThreadEntry, "Sensor Thread", 8192, NULL, 2, NULL, 1);
}

void loop() {}

void initPreferenceValues() {
  // Retrieve temperature preference from persisted memory
  // 0 = FER, 1 = CEL
  temperatureScale = (TemperatureScale)preferences.getInt("mes-sys", 0); // Default is Ferinehight
  plant.setSelectedPlant(preferences.getString("plant", "Generic"));
}

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
  // Temperature and Humidity Sensor
  hts = HumiditySensor(TEMP_ADDRESS, "Humidity and Temperature", "ea825233-6829-4ba3-b907-f6ab8d0a0e9e");
  Serial.printf("Humidity and Temp Sensor initialized:\n %d", hts.begin());

  // Light sensor
  lightSensor = LightSensor(LIGHT_ADDRESS, "Light", "light-sensor-01");
  Serial.printf("Light Sensor initialized: %d\n", lightSensor.begin());

  // Moisture sensor
  moistureSensor = MoistureSensor(SOIL_ADDRESS, "Soil Moisture", "moisture-sensor-01");
  Serial.printf("Light Sensor initialized: %d\n", moistureSensor.begin());

  
  display = Display(DISPLAY_ADDRESS, "Display", "display-01", &plant);
  display.begin();
  display.drawBootScreen();
  // delay(5000);
}

// Sensor threading
void sensorThreadEntry(void *pvParameters) {
  JsonDocument dataToTransmit;
  int count = 0;
 while(1) {
    if(count == 2) {
      plant.setSelectedPlant("Pothos (Golden)");
      preferences.putString("plant", "Pothos (Golden)");
    }
    String data;
    
    // Read in data from sensor
    hts.readData();
    lightSensor.readData();
    moistureSensor.readData();

    // Format to JSON for ease of use
    dataToTransmit.add(hts.parseData());
    dataToTransmit.add(lightSensor.parseData());
    dataToTransmit.add(moistureSensor.parseData());

    // serializeJsonPretty(dataToTransmit, Serial);

    // Serialize for transmission
    serializeJson(dataToTransmit, data);
    results = Util::formatConditions(&dataToTransmit);
  
    display.drawScreen(results, connectionStatus, temperatureScale);

    // Only transmit data when clients are connected
    if(pServer->getConnectedCount() > 0) {
      // Set value and notify client of new data
      pCharacteristic->setValue(data.c_str());
      pCharacteristic->notify();
    }
    dataToTransmit.clear();
    // Wait 5 seconds
    count++;
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