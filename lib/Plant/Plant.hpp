#pragma once
#include <Arduino.h>
#include <ArduinoJson.h>

struct Conditions {
    float temperatureMin;
    float temperatureMax;
    float humidityMin;
    float humidityMax;
    float lightMin;
    float lightMax;
    float soilMoistureMin;
    float soilMoistureMax;
};

struct ActualConditions {
    float temperature;
    float humidity;
    float light;
    float soilMoisture;
};

class Plant {
    private:
        String name;
        struct Conditions conditions;

    public:
        Plant();
        Plant(JsonDocument plantDatabase);
        Plant(String name, JsonDocument plantDatabase);
        String getName();
        boolean isHealthy(struct ActualConditions actualConditions);
};
