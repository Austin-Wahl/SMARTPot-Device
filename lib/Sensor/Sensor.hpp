/*
All sensors are derived from this super class. This class is designed to streamline the formatting of data.
*/

#ifndef SENSOR_H
#define SENSOR_H

#include <Arduino.h>
#include <ArduinoJson.h>

enum SensorType {
  TEMPERATURE_AND_HUMIDITY = 0
};

class Sensor {
protected:
    String name;
    String id;
    boolean connected;
    SensorType sensorType;
public:
    Sensor() {}
    Sensor(String name, String id, enum SensorType sensorType)
        : name(name), id(id), sensorType(sensorType) {}

    virtual ~Sensor() {}

    String getName() const { return name; }
    String getId()   const { return id; }

    boolean getConnected()   const { return id; }
    void setConnected(boolean status);
    enum SensorType getSensorType()   const { return sensorType; }

    virtual void readData() = 0;
    void parseData(JsonDocument& doc);
};

#endif