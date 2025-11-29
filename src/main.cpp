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
#define BLE_SEND_DATA_CHARACTERISTIC_ID "46f45f15-b963-4e4e-bde9-6a9a677df4b4"
#define BLE_SEND_UUID_CHARACTERISTIC_ID "4556f68a-e305-4ad2-aa34-e702e53a4b11"
#define BLE_RECIEVE_CONFIG_CHARACTERISTIC_ID "b645f869-3e5d-45ef-b1b6-2c17e0abe75c"
#define BLE_SEND_NOTIF_CHARACTERISTIC_ID "fcd0c4ed-e302-4adc-9f50-71f0de4e6045"

// Every SMARTPot has a unique UUID that is used to tie it to a database record in the app.
// Be sure to change this if you're flashing another pot. 
const char DEVICE_UUID[] = "5fa2b405-f5ef-42d3-88cd-8ab1f80d0611";

// These are the addresses for the sensors 
#define DISPLAY_ADDRESS 0x3C
#define TEMP_ADDRESS 0x40
#define LIGHT_ADDRESS 0x23
#define SOIL_ADDRESS 0x36

// These are pins
#define RELAY_PIN 5
#define WATER_SENSOR_PIN 32

Plant plant;
BLEService *pService;
BLECharacteristic *pCharacteristicSend;
BLECharacteristic *pCharacteristicRecieve;
BLECharacteristic *pCharacteristicDeviceId;
BLECharacteristic *pCharacteristicNotification;

struct ActualConditions results;
String connectionStatus = "Disconnected";

// Method signatures
void bluetoothThreadEntry(void *pvParameters);
void sensorThreadEntry(void *pvParameters);
void bluetoothSetup();
void sensorSetup();
boolean loadPlantDatabaseIntoMemory();
void initPreferenceValues();
void wateringController(int moisture);

// Global Variables 
JsonDocument plantDatabase;
HumiditySensor hts;
LightSensor lightSensor;
MoistureSensor moistureSensor;
Display display;
BLEServer *pServer;
TemperatureScale temperatureScale = FERINEHIGHT;
Preferences preferences;
int waterLevel = 0;

void setup() {
  Serial.begin(115200);
  pinMode(RELAY_PIN, OUTPUT);
  preferences.begin("prefs", false);

  pinMode(WATER_SENSOR_PIN, INPUT_PULLUP);
  digitalWrite(RELAY_PIN, LOW);
  boolean status = loadPlantDatabaseIntoMemory();
  if(!status) {
    return;
  }

  plant = Plant(&plantDatabase, &preferences);
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

  // Create Characteristics
  // This one is for sending sensor data
  pCharacteristicSend = pService->createCharacteristic(BLE_SEND_DATA_CHARACTERISTIC_ID, 
    BLECharacteristic::PROPERTY_READ |
    BLECharacteristic::PROPERTY_NOTIFY
  );
  pCharacteristicSend->addDescriptor(new BLE2902());

  // This one sends out the devices unique UUID
  pCharacteristicDeviceId = pService->createCharacteristic(BLE_SEND_UUID_CHARACTERISTIC_ID, BLECharacteristic::PROPERTY_READ);
  pCharacteristicDeviceId->addDescriptor(new BLE2902());
  pCharacteristicDeviceId->setValue(DEVICE_UUID);

  // This one is for sending out notifications
  pCharacteristicNotification = pService->createCharacteristic(BLE_SEND_NOTIF_CHARACTERISTIC_ID,
    BLECharacteristic::PROPERTY_READ |
    BLECharacteristic::PROPERTY_NOTIFY
  );
  pCharacteristicNotification->addDescriptor(new BLE2902());

  CharacteristicCallbacks *bleCharCb = new CharacteristicCallbacks(&plantDatabase, &plant, &temperatureScale, pCharacteristicNotification, &preferences);

  // Characteristic for reading in device config over bluetooth
  pCharacteristicRecieve = pService->createCharacteristic(BLE_RECIEVE_CONFIG_CHARACTERISTIC_ID,    
    BLECharacteristic::PROPERTY_READ |
    BLECharacteristic::PROPERTY_NOTIFY | 
    BLECharacteristic::PROPERTY_WRITE
  );
  pCharacteristicRecieve->addDescriptor(new BLE2902());
  pCharacteristicRecieve->setCallbacks(bleCharCb);
  
  BLECallbacks *cb = new BLECallbacks(bleCharCb, results, &display, connectionStatus, &temperatureScale, &waterLevel);
  pServer->setCallbacks(cb);

  pService->start();
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(BLE_SERVICE_UUID);

  uint8_t uuidBytes[16];
  sscanf(DEVICE_UUID, "%02hhx%02hhx%02hhx%02hhx-%02hhx%02hhx-%02hhx%02hhx-%02hhx%02hhx-%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx",
        &uuidBytes[0], &uuidBytes[1], &uuidBytes[2], &uuidBytes[3],
        &uuidBytes[4], &uuidBytes[5], &uuidBytes[6], &uuidBytes[7],
        &uuidBytes[8], &uuidBytes[9], &uuidBytes[10], &uuidBytes[11],
        &uuidBytes[12], &uuidBytes[13], &uuidBytes[14], &uuidBytes[15]);
  BLEAdvertisementData d;
  d.setManufacturerData(std::string((char *) uuidBytes, 16));

  pAdvertising->setAdvertisementData(d);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06); 
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
}

// Sensor configuration
void sensorSetup() {
  // Temperature and Humidity Sensor
  hts = HumiditySensor(TEMP_ADDRESS, "Humidity and Temperature", "ea825233-6829-4ba3-b907-f6ab8d0a0e9e");
  hts.begin();

  // Light sensor
  lightSensor = LightSensor(LIGHT_ADDRESS, "Light", "light-sensor-01");
  lightSensor.begin();

  // Moisture sensor
  moistureSensor = MoistureSensor(SOIL_ADDRESS, "Soil Moisture", "moisture-sensor-01");
  moistureSensor.begin();
  
  display = Display(DISPLAY_ADDRESS, "Display", "display-01", &plant);
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
    lightSensor.readData();
    moistureSensor.readData();

    // lowkey not makikng a class for this
    waterLevel = digitalRead(WATER_SENSOR_PIN);
    JsonDocument doc;
    JsonDocument metadataDoc;

    doc["name"] = "Water Level";
    doc["id"] = "Water Level 1";
    doc["connected"] = true;
    doc["data"]["level"] = waterLevel == HIGH ? "Low" : "High";

    
    // Format to JSON for ease of use
    dataToTransmit.add(hts.parseData());
    dataToTransmit.add(lightSensor.parseData());
    dataToTransmit.add(moistureSensor.parseData());
    dataToTransmit.add(doc);
    
    results = Util::formatConditions(&dataToTransmit);
    // calculate health score
    metadataDoc["metadata"]["health"] = plant.calculateHealthScore(results);
    metadataDoc["metadata"]["unitOfMeasurement"] = temperatureScale;
    dataToTransmit.add(metadataDoc);


    // Serialize for transmission
    serializeJson(dataToTransmit, data);

    display.drawScreen(results, connectionStatus, &temperatureScale, &waterLevel);

    // Trigger watering
    wateringController((int)results.soilMoisture);

    // Only transmit data when clients are connected
    if(pServer->getConnectedCount() > 0) {
      // Set value and notify client of new data
      pCharacteristicSend->setValue(data.c_str());
      pCharacteristicSend->notify();
    }
    dataToTransmit.clear();
    // Wait 5 seconds
    vTaskDelay(pdMS_TO_TICKS(SENSOR_READ_DURATION_MS));
  }
}

void wateringController(int soilMoisture) {
  // LOGIC INVERTED LOW = HIGH WATER
  //                HIGH = LOW WATER
  if(waterLevel == LOW && soilMoisture > -1) {
    // Watering is trigged if the soil is to dry
    // Get the plant record
    int soilMin = plantDatabase[plant.getSelectedPlant()]["soilMoistureMin"].as<int>();
    if(soilMoisture < soilMin) {
      digitalWrite(RELAY_PIN, HIGH);
    } else {
      digitalWrite(RELAY_PIN, LOW);
    }
  } else {
    digitalWrite(RELAY_PIN, LOW);
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