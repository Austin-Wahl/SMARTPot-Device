#pragma once
#include <Arduino.h>
#include <ArduinoJson.h>
#include <Util.hpp>

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

class Plant {
    private:
        String name;
        struct Conditions conditions;
        Weights computeWeights(const ActualConditions& actual);
        double metricScore(double value, Range ideal);
        String selectedPlant;
        JsonDocument *pPlantDatabase;
    public:
        Plant();
        Plant(JsonDocument *plantDatabase);
        String getSelectedPlant();
        void setSelectedPlant(String plant);
        String getName();
        double calculateHealthScore(struct ActualConditions& actualConditions);
};
