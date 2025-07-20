// LEDControl.cpp
// This file is part of the ESP32 OTA Test project.

#include "LEDControl.h"
#include <Arduino.h>

LEDControl::LEDControl(uint8_t redPin, uint8_t greenPin, uint8_t bluePin)
    : rPin(redPin), gPin(greenPin), bPin(bluePin),
      brightnessR(0), brightnessG(0), brightnessB(0) {}

void LEDControl::begin() {
    ledcSetup(0, 5000, 8); // Red
    ledcSetup(1, 5000, 8); // Green
    ledcSetup(2, 5000, 8); // Blue

    ledcAttachPin(rPin, 0);
    ledcAttachPin(gPin, 1);
    ledcAttachPin(bPin, 2);

    ledcWrite(0, 0); // Initialize Red LED
    ledcWrite(1, 0); // Initialize Green LED
    ledcWrite(2, 0); // Initialize Blue LED

    Serial.println("LED Control Initialized");
}

void LEDControl::update(float tempF, float tempC, float humidity) {
    brightnessR = mapAndConstrain(tempF, 77, 80);
    brightnessG = mapAndConstrain(humidity, 50, 70);
    brightnessB = mapAndConstrain(tempC, 30, 35);

    ledcWrite(0, brightnessR);
    ledcWrite(2, brightnessG);
    ledcWrite(1, brightnessB);
}

void LEDControl::debugPrint() const {
    Serial.print("💡 RED LED Brightness: ");
    Serial.println(brightnessR);
    Serial.print("💡 Green LED Brightness: ");
    Serial.println(brightnessG);
    Serial.print("💡 Blue LED Brightness: ");
    Serial.println(brightnessB);
}

int LEDControl::mapAndConstrain(float value, float in_min, float in_max) {
    int raw = map(value, in_min, in_max, 50, 255);
    return constrain(raw, 0, 255);
}