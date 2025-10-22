#include "BLECallbacks.hpp"
#include "HumiditySensor.hpp"
#include "Plant.hpp"
#include "FS.h"
#include "LittleFS.h"
#include "BLECallbacks.hpp"

#define HUMIDITY_SENSOR_PIN 23
#define SENSOR_READ_DURATION_MS 5000  // Time in MS between sensor readings
#define BLE_NAME "SmartPot"
#define BLE_SERVICE_UUID "6360ec7b-a2b6-41d2-87c6-be45caf92838"
#define BLE_CHARACTERISTIC_UUID "46f45f15-b963-4e4e-bde9-6a9a677df4b4"


Plant plant;

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


void setup() {
  Serial.begin(115200);

  boolean status = loadPlantDatabaseIntoMemory();
  if(!status) {
    return;
  }

  plant = Plant(plantDatabase);

  Serial.println("Hey");
  sensorSetup();  
  bluetoothSetup();

  xTaskCreatePinnedToCore(sensorThreadEntry, "Sensor Thread", 4096, NULL, 2, NULL, 1);
}

void loop() {}

// Bluetooth configuration
void bluetoothSetup() {
  // Set name
  BLEDevice::init(BLE_NAME);

  // Create server
  BLEServer *pServer = BLEDevice::createServer();

  // Create Service 
  BLEService *pService = pServer->createService(BLE_SERVICE_UUID);

  // Create Characteristic 
  BLECharacteristic *pCharacteristic = pService->createCharacteristic(BLE_CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);

  BLECallbacks *cb = new BLECallbacks();
  pServer->setCallbacks(cb);

  pCharacteristic->setValue("Hello World");
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
    JsonDocument doc;
    // If a sensor isn't connected, don't read the data from it.
    if(!hts.getConnected()) {
      Serial.println("HTS Sensor not connected...");
      hts.readData();
      vTaskDelay(pdMS_TO_TICKS(SENSOR_READ_DURATION_MS));
      continue;
    };

    hts.readData();

    JsonDocument data = hts.parseData()["data"];
    struct ActualConditions testDataConditions = formatConditions(TEMPERATURE_AND_HUMIDITY, data);

    vTaskDelay(pdMS_TO_TICKS(SENSOR_READ_DURATION_MS));
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