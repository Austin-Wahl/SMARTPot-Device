
#include <Util.hpp>

struct ActualConditions Util::formatConditions(JsonDocument data) {
 ActualConditions conditions;
  conditions.humidity = -1;
  conditions.light = -1;
  conditions.soilMoisture = -1;
  conditions.temperature = -1;

  for(int i = 0; i<data.size(); i++) {
    String tempName = data[i]["name"];
    if(tempName.equals("Humidity and Temperature")) {
      conditions.humidity = data[i]["data"]["humidity"] | 0.0f;
      conditions.temperature = data[i]["data"]["temperature"] | 0.0f;   
    }
  }

  return conditions;
}