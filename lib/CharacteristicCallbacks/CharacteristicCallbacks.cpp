#include <CharacteristicCallbacks.hpp>

CharacteristicCallbacks::CharacteristicCallbacks() {};

void CharacteristicCallbacks::onStatus(BLECharacteristic *pCharacteristic,Status s, uint32_t code) {
    Serial.println(s);
}