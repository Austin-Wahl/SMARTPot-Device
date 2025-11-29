#pragma once
#include <Arduino.h>
#include <BLECharacteristic.h>
#include <ArduinoJson.h>
#include <Plant.hpp>
#include <Preferences.h>

class CharacteristicCallbacks : public BLECharacteristicCallbacks {
    private:
        JsonDocument *pPlantDatabase;
        Plant *pPlant;
        TemperatureScale *pTemperatureScale;
        BLECharacteristic *pCharacteristicNotify;
        Preferences *pPreferences;
    public:
        CharacteristicCallbacks(JsonDocument *pPlantDatabase, Plant *pPlant, TemperatureScale *pTemperatureScale, BLECharacteristic *pCharacteristicNotify, Preferences *pPreferences);
        void onStatus(BLECharacteristic *pCharacteristic,Status s, uint32_t code);
        void onWrite(BLECharacteristic *pCharacteristic);
};