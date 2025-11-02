#pragma once
#include <Sensor.hpp>
#include <hp_BH1750.hpp>
#include <ArduinoJson.hpp>

struct LightSensorData {
    float lux;
};

class LightSensor : public Sensor {
    private:
        hp_BH1750 sensor;
        LightSensorData sensorData;

    public:
        LightSensor();
        LightSensor(uint8_t addr, String name, String id);

        void readData();
        
        boolean begin();

        JsonDocument parseData();
};