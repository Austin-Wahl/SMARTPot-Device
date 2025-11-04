
#include <Util.hpp>

struct ActualConditions Util::formatConditions(JsonDocument data) {
 ActualConditions conditions;
  conditions.humidity = -1;
  conditions.light = -1;
  conditions.soilMoisture = -1;
  conditions.soilTemperature = -1;
  conditions.temperature = -1;

  for(int i = 0; i<data.size(); i++) {
    String tempName = data[i]["name"];
    if(tempName.equals("Humidity and Temperature")) {
      conditions.humidity = data[i]["data"]["humidity"] | -1.0f;
      conditions.temperature = data[i]["data"]["temperature"] | -1.0f;   
    } else if(tempName.equals("Light")) {
      conditions.light = data[i]["data"]["light"] | -1.0f;   
    } else if(tempName.equals("Soil Moisture")) {
      conditions.soilTemperature = data[i]["data"]["temperature"] | -1.0f;   
      conditions.soilMoisture = data[i]["data"]["moisture"] | -1.0f;   
    }
  }

  return conditions;
}

String Util::soilMoistureToString(int rawMoisture) {
  if(rawMoisture <= 200) return "Very Dry";
  if(rawMoisture <= 700) return "Dry";
  if(rawMoisture <= 1000) return "Damp";
  if(rawMoisture <= 1300) return "Wet";
  if(rawMoisture <= 1600) return "Very Wet";
  if(rawMoisture <= 2000) return "Saturated";
}

double Util::celciusToFerinehight(double temp) {
  if(temp == -1) return -1;
  return (temp * 9/5) + 32;
}