// SensorHandler.cpp
// This file is part of the ESP32 OTA Test project.

#include "SensorHandler.h"

SensorHandler::SensorHandler(uint8_t pin, uint8_t type)
    : sensor(pin, type) {}

void SensorHandler::begin() {
    sensor.begin();
}

float SensorHandler::getTemperatureC() {
    return sensor.readTemperature();
}

float SensorHandler::getTemperatureF() {
    return sensor.readTemperature(true);
}

float SensorHandler::getHumidity() {
    return sensor.readHumidity();
}