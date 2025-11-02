#pragma once
#include <Arduino.h>
#include <ArduinoJson.hpp>
#include <Sensor.hpp>
#include "Adafruit_seesaw.h"

struct MoistureSensorData {
    float moisture;
    float temperature;
};

class MoistureSensor : public Sensor {
    private:
        Adafruit_seesaw sensor;
        MoistureSensorData sensorData;
    
    public:
        MoistureSensor();
        MoistureSensor(uint8_t addr, String name, String id);

        boolean begin();
        void readData();
        JsonDocument parseData();
};