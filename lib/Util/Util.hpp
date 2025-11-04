#pragma once
#include <Arduino.h>
#include <ArduinoJson.h>
#include "LittleFS.h"
#include <Plant.hpp>

enum TemperatureScale {
    FERINEHIGHT = 0,
    CELCIUS = 1
};

class Util {
    public:
        static struct ActualConditions formatConditions(JsonDocument data);
        static String soilMoistureToString(int rawMoisture);
        static double celciusToFerinehight(double temp);
};