#pragma once

#include <Arduino.h>
#include <BLEDevice.h>  // BLE LIB
#include <BLEUtils.h>   // BLE LIB
#include <BLEServer.h>  // BLE LIB
#include <CharacteristicCallbacks.hpp>

class BLECallbacks : public BLEServerCallbacks {
   private:
      CharacteristicCallbacks *cb;
      int led;
   public:
      BLECallbacks(CharacteristicCallbacks *cb, int bluetooth_led_pin_number) {
         this->cb = cb;
         this->led = bluetooth_led_pin_number;
      }
      int getLedPin() {
         return this->led;
      }
      void onConnect(BLEServer *pServer) override;
      void onDisconnect(BLEServer *pServer) override;
      CharacteristicCallbacks* getCharacteristicCallback() {
         return this->cb;
      }
};