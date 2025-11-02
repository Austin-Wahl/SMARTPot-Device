/*
All sensors are derived from this super class. This class is designed to streamline the formatting of data.
*/

#pragma once
#include <Arduino.h>
#include <ArduinoJson.h>

enum SensorType {
  TEMPERATURE_AND_HUMIDITY = 0,
  LIGHT                    = 1,
  MOISTURE                 = 2
};

class Sensor {
    protected:
        String name;
        String id;
        boolean connected;
        SensorType sensorType;
        uint8_t addr;
    public:
        Sensor() {}
        Sensor(uint8_t addr, String name, String id, enum SensorType sensorType)
            : addr(addr), name(name), id(id), sensorType(sensorType) {}

        virtual ~Sensor() {}
        
        
        String getName() const { return name; }
        String getId()   const { return id; }
        uint8_t getAddr() const {return addr;}

        boolean getConnected()   const { return connected; }
        void setConnected(boolean status) {connected = status;}
        enum SensorType getSensorType()   const { return sensorType; }

        virtual void readData() = 0;
        virtual boolean begin() = 0;
        JsonDocument parseData();
};