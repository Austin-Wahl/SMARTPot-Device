#include <LightSensor.hpp>

LightSensor::LightSensor(){}
LightSensor::LightSensor(uint8_t addr, String name, String id) : Sensor(addr, name, id, LIGHT) {
    sensor = hp_BH1750();
}

boolean LightSensor::begin() {
    boolean began = sensor.begin(addr);
    if(!began) return false;
    sensor.start();
    return began;
}

void LightSensor::readData() {
    if(sensor.hasValue()) {
        sensorData.lux = sensor.getLux();
        sensor.start();
    }
}

JsonDocument LightSensor::parseData() {
    JsonDocument data;
    JsonDocument doc;

    float lux = sensorData.lux;

    // Connection is checked everytime the data is parsed
    Wire.beginTransmission(this->getAddr());
    byte error = Wire.endTransmission();

    if(error == 0) {
        this->setConnected(true);
    } else {
        this->setConnected(false);
    }

    data["light"] = connected ? lux : -1;

    doc["name"] = this->getName();
    doc["id"] = this->getId();
    doc["connected"] = this->getConnected();
    doc["data"] = data;

    return doc;
}
