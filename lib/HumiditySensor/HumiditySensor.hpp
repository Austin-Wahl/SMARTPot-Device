#pragma once
#include <Sensor.hpp>
#include <DHTesp.h>     // TEMPERATURE SENSOR LIB

class HumiditySensor : public Sensor {
    private:
        DHTesp sensor;
        TempAndHumidity sensorData;
        boolean connected;
        
    public:
        HumiditySensor();
        HumiditySensor(uint8_t pin, String name, String id); 

        void readData();

        void setConnected(boolean status);

        boolean getConnected();

        JsonDocument parseData();
};
