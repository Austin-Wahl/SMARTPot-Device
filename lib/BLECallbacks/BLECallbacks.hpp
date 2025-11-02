#pragma once

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <CharacteristicCallbacks.hpp>
#include <Display.hpp>

class BLECallbacks : public BLEServerCallbacks {
   private:
      CharacteristicCallbacks *cb;
      struct ActualConditions &tempValues;
      Display *display;
      String &connectionStatus;

   public:
      BLECallbacks(CharacteristicCallbacks *cb,
                   struct ActualConditions &tempValues,
                   Display *display, String &connectionStatus)
          : cb(cb), tempValues(tempValues), display(display), connectionStatus(connectionStatus) {
      }

      void onConnect(BLEServer *pServer) override;
      void onDisconnect(BLEServer *pServer) override;

      struct ActualConditions getTempValues() { return tempValues; }

      Display *getDisplay() { return display; }
      String &getConnectionStatus() {return connectionStatus;}
      void setConnectionStauts(String newStatus) {connectionStatus = newStatus;}
      CharacteristicCallbacks *getCharacteristicCallback() {
         return cb;
      }
};