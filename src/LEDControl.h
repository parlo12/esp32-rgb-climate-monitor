// LEDControl.h
// This file is part of the ESP32 OTA Test project.
#ifndef LED_CONTROLLER_H
#define LED_CONTROLLER_H

#include <Arduino.h>

class LEDControl {
public:
    LEDControl(uint8_t redPin, uint8_t greenPin, uint8_t bluePin);

    void begin();
    void update(float tempF, float tempC, float humidity);
    void debugPrint() const;

private:
    uint8_t rPin, gPin, bPin;
    int brightnessR, brightnessG, brightnessB;

    int mapAndConstrain(float value, float in_min, float in_max);
};
#endif