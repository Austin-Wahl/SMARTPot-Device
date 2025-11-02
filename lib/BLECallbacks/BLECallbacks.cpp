#include <BLECallbacks.hpp>
#include <Arduino.h>
void BLECallbacks::onConnect(BLEServer *pServer) {
   Serial.println("Client Connected..");
   struct ActualConditions temp = this->getTempValues();
   this->setConnectionStauts("Connected");
   this->getDisplay()->drawScreen(temp, this->getConnectionStatus());
}

void BLECallbacks::onDisconnect(BLEServer *pServer) {
      Serial.println("Client Disconnected");
      struct ActualConditions temp = this->getTempValues();
      this->setConnectionStauts("Disconnected");
      this->getDisplay()->drawScreen(temp, this->getConnectionStatus());
      delay(1000);
      pServer->startAdvertising();
}
