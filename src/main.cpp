#include "BLECallbacks.hpp"
#include "HumiditySensor.hpp"
#include "Plant.hpp"
#include "FS.h"
#include "LittleFS.h"
#include "BLECallbacks.hpp"
#include "CharacteristicCallbacks.hpp"
#include "BLE2902.h"
#include "Wire.h"

#define HUMIDITY_SENSOR_PIN 32
#define GREEN_LED_POWER_INDICATOR_PIN 27
#define BLUE_LED_BLUETOOTH_INDICATOR_PIN 26
#define YELLOW_LED_SENDING_BDATA 25
#define SENSOR_READ_DURATION_MS 5000  // Time in MS between sensor readings
#define BLE_NAME "SmartPot"
#define BLE_SERVICE_UUID "6360ec7b-a2b6-41d2-87c6-be45caf92838"
#define BLE_CHARACTERISTIC_UUID "46f45f15-b963-4e4e-bde9-6a9a677df4b4"

// These are the addresses for the sensors 
#define DISPLAY_ADDRESS 0x3D
#define TEMP_ADDRESS 0x40
#define LIGHT_ADDRESS 0x23
#define SOIL_ADDRESS 0x36

Plant plant;
BLEService *pService;
BLECharacteristic *pCharacteristic;

// Method signatures
void bluetoothThreadEntry(void *pvParameters);
void sensorThreadEntry(void *pvParameters);
void bluetoothSetup();
void sensorSetup();
boolean loadPlantDatabaseIntoMemory();
struct ActualConditions formatConditions(enum SensorType sensorType, JsonDocument data);

// Global Variables 
JsonDocument plantDatabase;
HumiditySensor hts;
BLEServer *pServer;

void setup() {
  Serial.begin(115200);

  pinMode(GREEN_LED_POWER_INDICATOR_PIN, OUTPUT);
  pinMode(BLUE_LED_BLUETOOTH_INDICATOR_PIN, OUTPUT);
  pinMode(YELLOW_LED_SENDING_BDATA, OUTPUT);

  boolean status = loadPlantDatabaseIntoMemory();
  if(!status) {
    return;
  }

  plant = Plant(plantDatabase);

  digitalWrite(GREEN_LED_POWER_INDICATOR_PIN, HIGH);

  bluetoothSetup();
  sensorSetup();  

  xTaskCreatePinnedToCore(sensorThreadEntry, "Sensor Thread", 4096, NULL, 2, NULL, 1);
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

  BLECallbacks *cb = new BLECallbacks(bleCharCb, BLUE_LED_BLUETOOTH_INDICATOR_PIN);
  pServer->setCallbacks(cb);

  // pCharacteristic->setValue("Hello World");

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
   hts = HumiditySensor((int)HUMIDITY_SENSOR_PIN, "Humidity and Temperature", "ea825233-6829-4ba3-b907-f6ab8d0a0e9e");
}

// Sensor threading
void sensorThreadEntry(void *pvParameters) {
 while(1) {
    String data;
    JsonDocument dataToTransmit = JsonDocument();
    int delayDur = SENSOR_READ_DURATION_MS;

    // Read in data from sensor
    hts.readData();

    // Format to JSON for ease of use
    dataToTransmit.add(hts.parseData());

    // Serialize for transmission
    serializeJson(dataToTransmit, data);

    // Only transmit data when clients are connected
    if(pServer->getConnectedCount() > 0) {
      digitalWrite(YELLOW_LED_SENDING_BDATA, HIGH);
      delayDur -= 1000;

      // Set value and notify client of new data
      pCharacteristic->setValue(data.c_str());
      pCharacteristic->notify();
      delay(1000);
      digitalWrite(YELLOW_LED_SENDING_BDATA, LOW);

    }

    // Wait 5 seconds
    vTaskDelay(pdMS_TO_TICKS(delayDur));
  }
}

struct ActualConditions formatConditions(enum SensorType sensorType, JsonDocument data) {
 ActualConditions conditions;
  conditions.humidity = -1;
  conditions.light = -1;
  conditions.soilMoisture = -1;
  conditions.temperature = -1;

  switch(sensorType) {
    case TEMPERATURE_AND_HUMIDITY:
      conditions.temperature = data["temperature"].as<float>();
      conditions.humidity = data["humidity"].as<float>();
      return conditions;
    default:  
      return conditions;
  }
}

// Loads the JSON database
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