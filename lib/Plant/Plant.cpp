#include <Plant.hpp>

Plant::Plant() {}
Plant::Plant(JsonDocument plantDatabase) {
    selectedPlant = "Generic";
    conditions.temperatureMin = 15.6;
    conditions.temperatureMax = 26.7;

    conditions.humidityMin = 30.0;
    conditions.humidityMax = 80.0;

    conditions.lightMin = 4000.0;
    conditions.lightMax = 12000.0;

    conditions.soilMoistureMin = 400.0;
    conditions.soilMoistureMax = 1300.0;
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
double Plant::calculateHealthScore(struct ActualConditions &actualConditions) {
  Weights w = computeWeights(actualConditions);

  struct Range temperatureRange;
  struct Range moistureRange;
  struct Range lightRange;
  struct Range humidityRange;

  temperatureRange.min = conditions.temperatureMin;
  temperatureRange.max = conditions.temperatureMax;
  
  moistureRange.min = conditions.soilMoistureMin;
  moistureRange.max = conditions.soilMoistureMax;

  lightRange.min = conditions.lightMin;
  lightRange.max = conditions.lightMax;

  humidityRange.min = conditions.humidityMin;
  humidityRange.max = conditions.humidityMax;

  double tScore = metricScore(actualConditions.temperature, temperatureRange);
  double hScore = metricScore(actualConditions.humidity, humidityRange);
  double sScore = metricScore(actualConditions.soilMoisture, moistureRange);
  double lScore = metricScore(actualConditions.light, lightRange);

  if (tScore < 0) tScore = 0;
  if (hScore < 0) hScore = 0;
  if (sScore < 0) sScore = 0;
  if (lScore < 0) lScore = 0;

  double health01 = w.temperature * tScore +
                    w.humidity    * hScore +
                    w.soil        * sScore +
                    w.light       * lScore;

  double health1to5 = 1.0 + 4.0 * health01;   
  return round(health1to5 * 10) / 10.0;
}

Weights Plant::computeWeights(const ActualConditions& actual) {
  double tW = 0.25;
  double hW = 0.15;
  double sW = 0.50;
  double lW = 0.10;

  if (actual.temperature == -1) tW = 0.0;
  if (actual.humidity == -1)    hW = 0.0;
  if (actual.soilMoisture == -1)        sW = 0.0;
  if (actual.light == -1)       lW = 0.0;

  double sum = tW + hW + sW + lW;
  if (sum == 0) sum = 1;
  double factor = 1.0 / sum;

  Weights w;
  w.temperature = tW * factor;
  w.humidity    = hW * factor;
  w.soil        = sW * factor;
  w.light       = lW * factor;
  return w;
}

double Plant::metricScore(double value, Range ideal) {
  if (value == -1) return -1;  
  if (value >= ideal.min && value <= ideal.max) return 1.0;

  double diff;
  if (value < ideal.min)
    diff = (ideal.min - value) / ideal.min;
  else
    diff = (value - ideal.max) / ideal.max;

  diff = std::min(diff, 1.0);
  return std::max(0.0, 1.0 - diff); 
}

void Plant::setSelectedPlant(String plant) {
    this->selectedPlant = plant;
}

String Plant::getSelectedPlant() {
    return this->selectedPlant;
}