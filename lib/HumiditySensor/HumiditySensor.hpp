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
        boolean connected;
        
    public:
        HumiditySensor();
        HumiditySensor(uint8_t addr, String name, String id); 

        void readData();

        void setConnected(boolean status);

        boolean begin();

        boolean getConnected();

        JsonDocument parseData();
};
