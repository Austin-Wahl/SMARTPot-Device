#include <HumiditySensor.hpp>

HumiditySensor::HumiditySensor() {};

HumiditySensor::HumiditySensor(uint8_t addr, String name, String id) : Sensor(addr, name, id, TEMPERATURE_AND_HUMIDITY) {
    sensor = Adafruit_HTU31D();
}    

boolean HumiditySensor::begin() {
    return sensor.begin(addr);
}

void HumiditySensor::readData() {
    sensors_event_t temp, hum;
    sensor.getEvent(&hum, &temp);
    sensorData.humidity = hum.relative_humidity;
    sensorData.temperature = temp.temperature;
}

JsonDocument HumiditySensor::parseData() {
    JsonDocument data;
    JsonDocument doc;

    float temp = sensorData.temperature;
    float humidity = sensorData.humidity;

    // Connection is checked everytime the data is parsed
    Wire.beginTransmission(this->getAddr());
    byte error = Wire.endTransmission();

    if(error == 0) {
        this->setConnected(true);
    } else {
        Serial.println(error);
        this->setConnected(false);
    }

    data["temperature"] = connected ? temp : -1;
    data["humidity"] = connected ? humidity : -1;

    doc["name"] = this->getName();
    doc["id"] = this->getId();
    doc["connected"] = this->getConnected();
    doc["data"] = data;

    return doc;
}       
