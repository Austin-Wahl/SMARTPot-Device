#include <BLECallbacks.hpp>
#include <Arduino.h>
void BLECallbacks::onConnect(BLEServer *pServer) {
   Serial.println("Client Connected..");
   digitalWrite(this->getLedPin(), HIGH);
}

void BLECallbacks::onDisconnect(BLEServer *pServer) {
      Serial.println("Client Disconnected");
      digitalWrite(this->getLedPin(), LOW);

      delay(1000);
      pServer->startAdvertising();
}
