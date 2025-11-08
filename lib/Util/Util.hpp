#pragma once
#include <Arduino.h>
#include <ArduinoJson.h>
#include "LittleFS.h"

enum TemperatureScale {
    FERINEHIGHT = 0,
    CELCIUS = 1
};

struct Conditions {
    float temperatureMin;
    float temperatureMax;
    float humidityMin;
    float humidityMax;
    float lightMin;
    float lightMax;
    float soilMoistureMin;
    float soilMoistureMax;
};

struct ActualConditions {
    float temperature;
    float humidity;
    float light;
    float soilMoisture;
    float soilTemperature;
};

class Util {
    public:
        static struct ActualConditions formatConditions(JsonDocument *data);
        static struct Conditions jsonToStruct(const JsonObject& object);
        static String soilMoistureToString(int rawMoisture);
        static double celciusToFerinehight(double temp);
};