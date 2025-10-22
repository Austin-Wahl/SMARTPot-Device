#include <Plant.hpp>

Plant::Plant() {}
Plant::Plant(JsonDocument plantDatabase) {
    name = "Generic";
    conditions.temperatureMin = 60.0;
    conditions.temperatureMax = 80.0;

    conditions.humidityMin = 30.0;
    conditions.humidityMax = 80.0;

    conditions.lightMin = 4000.0;
    conditions.lightMax = 12000.0;

    conditions.soilMoistureMin = 35.0;
    conditions.soilMoistureMax = 60.0;
}

Plant::Plant(String name, JsonDocument plantDatabase) {
    this->name = name;

    JsonDocument tempPlantData = plantDatabase[name];

    struct Conditions c;

    c.temperatureMin   = tempPlantData["temperatureMin"]   | 0.0f;
    c.temperatureMax   = tempPlantData["temperatureMax"]   | 0.0f;
    c.humidityMin      = tempPlantData["humidityMin"]      | 0.0f;
    c.humidityMax      = tempPlantData["humidityMax"]      | 0.0f;
    c.lightMin         = tempPlantData["lightMin"]         | 0.0f;
    c.lightMax         = tempPlantData["lightMax"]         | 0.0f;
    c.soilMoistureMin  = tempPlantData["soilMoistureMin"]  | 0.0f;
    c.soilMoistureMax  = tempPlantData["soilMoistureMax"]  | 0.0f;

    conditions = c;
}

String Plant::getName() {
    return this->name;
}
boolean Plant::isHealthy(struct ActualConditions actualConditions) {
    if(actualConditions.humidity != -1) {
        if(actualConditions.humidity > conditions.humidityMax) {
            Serial.println("Humidity to high");
            return false;
        }
        if(actualConditions.humidity < conditions.humidityMin) {
            Serial.println("Humidity to low");
            return false;
        }
    }

    if(actualConditions.light != -1) {
        if(actualConditions.light > conditions.lightMax) {
            Serial.println("light to high");
            return false;
        }
        if(actualConditions.light < conditions.lightMin) {
            Serial.println("light to low");
            return false;
        }
    }

    if(actualConditions.soilMoisture != -1) {
        if(actualConditions.soilMoisture > conditions.soilMoistureMax) {
            Serial.println("soilMoisture to high");
            return false;
        }
        if(actualConditions.soilMoisture < conditions.soilMoistureMin) {
            Serial.println("soilMoisture to low");
            return false;
        }
    }

    if(actualConditions.temperature != -1) {
        if(actualConditions.temperature > conditions.temperatureMax) {
            Serial.println("temperature to high");
            return false;
        }
        if(actualConditions.temperature < conditions.temperatureMin) {
            Serial.println("temperature to low");
            return false;
        }
    }

    return true;
}