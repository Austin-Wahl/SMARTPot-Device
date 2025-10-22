#include <BLECallbacks.hpp>

void BLECallbacks::onConnect(BLEServer *pServer) {
   Serial.println("Client Connected..");
}

void BLECallbacks::onDisconnect(BLEServer *pServer) {
      Serial.println("Client Disconnected");
      delay(1000);
      pServer->startAdvertising();
}
