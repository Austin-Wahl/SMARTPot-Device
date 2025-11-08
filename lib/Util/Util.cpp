
#include <Util.hpp>

struct ActualConditions Util::formatConditions(JsonDocument *data) {
 ActualConditions conditions;
  conditions.humidity = -1;
  conditions.light = -1;
  conditions.soilMoisture = -1;
  conditions.soilTemperature = -1;
  conditions.temperature = -1;

  if(data == nullptr || data->isNull()) return conditions;

  JsonArray sensorDataArray = data->as<JsonArray>();

  for(JsonObject sensorEntry : sensorDataArray) {
    String tempName = sensorEntry["name"];
    JsonObject dataPayload = sensorEntry["data"].as<JsonObject>();
    if(tempName.equals("Humidity and Temperature")) {
      conditions.humidity = dataPayload["humidity"] | -1.0f;
      conditions.temperature = dataPayload["temperature"] | -1.0f;   
    } else if(tempName.equals("Light")) {
      conditions.light = dataPayload["light"] | -1.0f;   
    } else if(tempName.equals("Soil Moisture")) {
      conditions.soilTemperature = dataPayload["temperature"] | -1.0f;   
      conditions.soilMoisture = dataPayload["moisture"] | -1.0f;   
    }
  }

  Serial.print(conditions.temperature);
  return conditions;
}

String Util::soilMoistureToString(int rawMoisture) {
  if(rawMoisture <= 200) return "Very Dry";
  if(rawMoisture <= 700) return "Dry";
  if(rawMoisture <= 1000) return "Damp";
  if(rawMoisture <= 1300) return "Wet";
  if(rawMoisture <= 1600) return "Very Wet";
  return "Saturated";
}

double Util::celciusToFerinehight(double temp) {
  if(temp == -1) return -1;
  return (temp * 9/5) + 32;
}

struct Conditions Util::jsonToStruct(const JsonObject& object) {
  Conditions c;

  c.temperatureMin   = object["temperatureMin"]   | 0.0f; // Using | 0.0f for default if key missing
  c.temperatureMax   = object["temperatureMax"]   | 0.0f;
  c.humidityMin      = object["humidityMin"]      | 0.0f;
  c.humidityMax      = object["humidityMax"]      | 0.0f;
  c.lightMin         = object["lightMin"]         | 0.0f;
  c.lightMax         = object["lightMax"]         | 0.0f;
  c.soilMoistureMin  = object["soilMoistureMin"]  | 0.0f;
  c.soilMoistureMax  = object["soilMoistureMax"];
  return c;
}