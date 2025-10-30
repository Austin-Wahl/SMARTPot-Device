#pragma once
#include <Arduino.h>
#include <BLECharacteristic.h>

class CharacteristicCallbacks : public BLECharacteristicCallbacks {
    public:
        CharacteristicCallbacks();
        void onStatus(BLECharacteristic *pCharacteristic,Status s, uint32_t code);
};