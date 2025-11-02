#pragma once
#include <Arduino.h>
#include <ArduinoJson.h>
#include "LittleFS.h"
#include <Plant.hpp>

class Util {
    public:
        static struct ActualConditions formatConditions(JsonDocument data);
        static String soilMoistureToString(int rawMoisture);
};