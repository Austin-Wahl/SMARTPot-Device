#pragma once
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Sensor.hpp> 
#include <Wire.h>
#include <Plant.hpp>
#include <Util.hpp>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)

class Display : public Adafruit_SSD1306 {
    private:
        uint8_t addr;
        String name;
        String id;
        Plant *pPlant;
    public:
        Display(){}
        Display(uint8_t addr, String name, String id, Plant *pPlant);
        
        boolean begin();
        String getId() const {return id;}
        String getName() const {return name;}
        uint8_t getAddr() const {return addr;} 
        void drawScreen(struct ActualConditions &tempValues, String connectionStatus, TemperatureScale *ts, int *pWaterLevel);
        void drawBluetoothLogo(String connectionStatus);       
        void drawBootScreen();
};