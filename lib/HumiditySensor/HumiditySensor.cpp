#include <HumiditySensor.hpp>

HumiditySensor::HumiditySensor() {};

HumiditySensor::HumiditySensor(uint8_t pin, String name, String id) : Sensor(name, id, TEMPERATURE_AND_HUMIDITY) {
    sensor.setup(pin, DHTesp::DHT11);
}    

void HumiditySensor::readData() {
    sensorData = sensor.getTempAndHumidity();
}

void HumiditySensor::setConnected(boolean status) {
    this->connected = status;
}

boolean HumiditySensor::getConnected() {
    return this->connected;
}

JsonDocument HumiditySensor::parseData() {
    JsonDocument data;
    JsonDocument doc;

    float temp = sensorData.temperature;
    float humidity = sensorData.humidity;

    data["temperature"] = (temp * 9/5) + 32;
    data["humidity"] = humidity;

    // Connection is checked everytime the data is parsed
    if(isnan(temp) && isnan(humidity)) {
        this->setConnected(false);
    } else {
        this->setConnected(true);
    }

    doc["name"] = this->getName();
    doc["id"] = this->getId();
    doc["connected"] = this->getConnected();
    doc["data"] = data;

    return doc;
}       
