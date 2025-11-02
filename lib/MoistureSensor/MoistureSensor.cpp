#include <MoistureSensor.hpp>

MoistureSensor::MoistureSensor() {}
MoistureSensor::MoistureSensor(uint8_t addr, String name, String id) : Sensor(addr, name, id, MOISTURE) {
    sensor = Adafruit_seesaw();
}

boolean MoistureSensor::begin() {
    return sensor.begin(addr);
}

void MoistureSensor::readData() {
    sensorData.temperature = sensor.getTemp();
    sensorData.moisture = sensor.touchRead(0);
}

JsonDocument MoistureSensor::parseData() {
    JsonDocument data;
    JsonDocument doc;

    float temp = sensorData.temperature;
    float moisture = sensorData.moisture;

    // Connection is checked everytime the data is parsed
    Wire.beginTransmission(this->getAddr());
    byte error = Wire.endTransmission();

    if(error == 0) {
        this->setConnected(true);
    } else {
        Serial.println(error);
        this->setConnected(false);
    }

    data["temperature"] = connected ? (temp * 9/5) + 32 : -1;
    data["moisture"] = connected ? moisture : -1;

    doc["name"] = this->getName();
    doc["id"] = this->getId();
    doc["connected"] = this->getConnected();
    doc["data"] = data;

    return doc;
}