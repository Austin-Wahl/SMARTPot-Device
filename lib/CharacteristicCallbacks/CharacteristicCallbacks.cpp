#include <CharacteristicCallbacks.hpp>

CharacteristicCallbacks::CharacteristicCallbacks(JsonDocument *pPlantDatabase, Plant *pPlant, TemperatureScale *pTemperatureScale, BLECharacteristic *pCharacteristicNotify, Preferences *pPreferences) {
    this->pPlantDatabase = pPlantDatabase;
    this->pPlant = pPlant;
    this->pTemperatureScale = pTemperatureScale;
    this->pCharacteristicNotify = pCharacteristicNotify;
    this->pPreferences = pPreferences;
};

void CharacteristicCallbacks::onStatus(BLECharacteristic *pCharacteristic,Status s, uint32_t code) {
}

void CharacteristicCallbacks::onWrite(BLECharacteristic* pCharacteristic) {
        std::string rxValue = pCharacteristic->getValue();

        JsonDocument doc;
        JsonDocument notification;
        String d;
        if (rxValue.length() > 0) {
            try {
                // Convert the json input from app to arduino json doc
                String deviceName;
                String plant;
                int measurementSystem;


                DeserializationError err = deserializeJson(doc, rxValue);
                if(err) throw std::runtime_error(err.c_str());

                // Read in the device name
                if(doc.containsKey("deviceName")) {
                    deviceName = doc["deviceName"].as<String>();

                    if(deviceName.length() <= 0) {
                        throw std::runtime_error("Device name is to short.");
                    } else if(deviceName.length() > 20) {
                        throw std::runtime_error("Device name is to long.");
                    }
                }

                if(doc.containsKey("plant")) {
                    plant = doc["plant"].as<String>();

                    if(plant.length() <= 0) {
                        throw std::runtime_error("Plant name is required.");
                    }

                    if((*pPlantDatabase)[plant].is<String>()) {
                        throw std::runtime_error("Invalid Plant Name provided.");
                    }

                    pPlant->setSelectedPlant(plant);
                }

                 if(doc.containsKey("mes_sys")) {
                    measurementSystem = doc["mes_sys"].as<int>();

                    if(measurementSystem != 0 && measurementSystem != 1) {
                        throw std::runtime_error("Invalid Measurement System.");
                    }

                    *pTemperatureScale = (TemperatureScale)measurementSystem;
                    pPreferences->putInt("mes-sys", *pTemperatureScale);
                }



                notification["error"] = NULL;
                notification["success"] = true;

                serializeJson(notification, d);
                pCharacteristicNotify->setValue(d.c_str());
                pCharacteristicNotify->notify(true);       
                
                serializeJsonPretty(doc, Serial);
            } catch (const std::runtime_error& e) { 
                notification["error"] = e.what();
                notification["success"] = false;

                serializeJson(notification, d);
                pCharacteristicNotify->setValue(d.c_str());
                pCharacteristicNotify->notify(true);
            } catch(...) {
                notification["error"] = "There was an issue parsing data on your SMARTPot.";
                notification["success"] = false;

                serializeJson(notification, d);
                pCharacteristicNotify->setValue(d.c_str());
                pCharacteristicNotify->notify(true);
            }
        }
}