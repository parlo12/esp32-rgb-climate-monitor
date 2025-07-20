// SensorHandler.h
// This file is part of the ESP32 OTA Test project.
#ifndef SENSOR_HANDLER_H
#define SENSOR_HANDLER_H

#include <Arduino.h>
#include "DHT.h"

class SensorHandler {
public: 
    // Constructor to initialize DHT sensor with pin and type
    SensorHandler(uint8_t pin, uint8_t type);
        void begin(); // Function to initialize the DHT sensor
        float getTemperatureC();
        float getTemperatureF();
        float getHumidity();

private:
    DHT sensor;
};

#endif
