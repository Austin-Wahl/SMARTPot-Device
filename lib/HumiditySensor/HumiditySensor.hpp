#pragma once
#include <Sensor.hpp>
#include <Adafruit_HTU31D.h>
#include <Wire.h>

struct TempAndHumidity {
    float temperature;
    float humidity;
};

class HumiditySensor : public Sensor {
    private:
        Adafruit_HTU31D sensor;
        TempAndHumidity sensorData;
        
    public:
        HumiditySensor();
        HumiditySensor(uint8_t addr, String name, String id); 

        void readData();

        boolean begin();

        JsonDocument parseData();
};
