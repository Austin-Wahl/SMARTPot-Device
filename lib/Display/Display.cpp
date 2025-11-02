#include <Display.hpp>

Display::Display(uint8_t addr, String name, String id, Plant *pPlant) : Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET) {
    this->id = id;
    this->name = name;
    this->addr = addr;
    this->pPlant = pPlant;
}
bool Display::begin() {
    return Adafruit_SSD1306::begin(SSD1306_SWITCHCAPVCC, addr);
}
void Display::drawScreen(struct ActualConditions &tempValues, String connectionStatus) {
  // Reset the display on every frame
  clearDisplay();
  setTextSize(1);

  drawBluetoothLogo(connectionStatus); // Draws the BT Logo
  setCursor(0,20);
  
  // Draws Temp and Humidity Sensor data
  if(tempValues.temperature == -1 && tempValues.humidity == -1) {
    println("Temperature: NC");
    println("Humidity: NC");
  } else {
    print("Temperature: ");
    print(tempValues.temperature, 2);
    println("F");

    print("Humidity: ");
    print(tempValues.humidity, 2);
    println("%");
  }

  if(tempValues.light == -1) {
    println("Light: NC");
  } else {
    print("Light: ");
    print(tempValues.light);
    println("LUX");
  }

    if(tempValues.soilMoisture == -1) {
        println("Moisture: NC");
    } else {
        print("Moisture: ");
        println(Util::soilMoistureToString(tempValues.soilMoisture));
    }

  print("Health: ");
  println(pPlant->isHealthy(tempValues));

  // Draws light sensor values

  display();
}

void Display::drawBluetoothLogo(String connectionStatus) {
  const unsigned char bluetooth_logo [] PROGMEM = {
    0x00, 0x00, 0x00, 0x80, 0x00, 0xc0, 0x00, 0xa0, 0x04, 0x90, 0x02, 0xa0, 0x01, 0xc0, 0x00, 0x80, 
    0x01, 0x80, 0x02, 0xc0, 0x04, 0xa0, 0x00, 0x90, 0x00, 0xa0, 0x00, 0xc0, 0x00, 0x80, 0x00, 0x00
  };
  // Init display
  setCursor(16,2);
  setTextColor(WHITE);
  println(connectionStatus);
  drawBitmap(-4,0, bluetooth_logo, 16, 14, WHITE);
}

void Display::drawBootScreen() {
    clearDisplay();
    setCursor(0,0);
    setTextColor(WHITE);
    setTextSize(1);
    println("Welcome to your");
    setTextSize(3);
    println("SMART");
    println("Pot");
    display();
}