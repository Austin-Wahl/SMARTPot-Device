#pragma once

#include <Arduino.h>
#include <BLEDevice.h>  // BLE LIB
#include <BLEUtils.h>   // BLE LIB
#include <BLEServer.h>  // BLE LIB
class BLECallbacks : public BLEServerCallbacks {
   void onConnect(BLEServer *pServer) override;
   void onDisconnect(BLEServer *pServer) override;
};