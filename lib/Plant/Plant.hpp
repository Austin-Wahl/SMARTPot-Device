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

struct Weights {
  double temperature;
  double humidity;
  double soil;
  double light;
};

struct Range {
  double min;
  double max;
};

struct ActualConditions {
    float temperature;
    float humidity;
    float light;
    float soilMoisture;
    float soilTemperature;
};

class Plant {
    private:
        String name;
        struct Conditions conditions;
        Weights computeWeights(const ActualConditions& actual);
        double metricScore(double value, Range ideal);
        String selectedPlant;
    public:
        Plant();
        Plant(JsonDocument plantDatabase);
        Plant(String name, JsonDocument plantDatabase);
        String getSelectedPlant();
        void setSelectedPlant(String plant);
        String getName();
        double calculateHealthScore(struct ActualConditions& actualConditions);
};
