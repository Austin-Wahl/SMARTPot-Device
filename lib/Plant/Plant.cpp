#include <Plant.hpp>

Plant::Plant() {}
Plant::Plant(JsonDocument *plantDatabase, Preferences *pPreferences) {
    pPlantDatabase = plantDatabase;
    conditions = Util::jsonToStruct((*plantDatabase)["Generic"]);
    this->pPreferences = pPreferences;
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
  double hW = 0.25;
  double sW = 0.25;
  double lW = 0.25;

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
    conditions = Util::jsonToStruct((*pPlantDatabase)[plant]);
    pPreferences->putString("plant", plant);
}

String Plant::getSelectedPlant() {
    return this->selectedPlant;
}