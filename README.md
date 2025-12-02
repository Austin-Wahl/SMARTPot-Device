# SMARTPot Microcontroller Code

This repository contains the C++ firmware for the ESP32-based SMARTPot device. It is designed for development using PlatformIO within Visual Studio Code.

# Getting Started

Familiarize yourself with the following key directories to understand the project structure:

- `/src/`

  - Source Code: This directory holds the main application logic, including the program's entry point (main.cpp).

- `/lib/`

  - Libraries: Contains custom-developed and imported external libraries. Each class is organized into its own subfolder, typically with a header (.h) and implementation (.cpp) file.

- `/data/plantprofiles.json`
  - Plant Profile Database: This JSON file stores preconfigured parameters and thresholds used to assess the health status of various plant environments.


# `main.cpp`

This file serves as the entry point for the firmware and contains two fundamental Arduino functions: `setup()` and `loop()`.

- **`void setup()`**: Executes once when the microcontroller board boots.
- **`void loop()`**: Designed to run continuously after `setup()` completes.

You'll notice that the `loop()` function in this project is empty. This is by design, as the application leverages **multi-threading** to concurrently handle tasks such as reading sensor data and managing Bluetooth communication.

Specifically, the `setup()` function initiates these concurrent operations by calling `xTaskCreatePinnedToCore()`. This function spawns a new FreeRTOS task, with `sensorThreadEntry` as its entry point. The `sensorThreadEntry` function is responsible for all sensor-related logic and data acquisition.
