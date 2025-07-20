// WiFiManager.cpp
// This file is part of the ESP32 OTA Test project.

#include "WiFiManager.h"
#include <Arduino.h>
#include <ArduinoOTA.h>

#ifndef BUILTIN_LED
#define BUILTIN_LED 2
#endif

WiFiManager::WiFiManager(const char* ssid, const char* password)
  : ssid(ssid), password(password) {}

void WiFiManager::connect() {
  Serial.println("🔌 Connecting to WiFi...");
  WiFi.begin(ssid, password);

  int attempts = 0; 
  const int maxAttempts = 20; 

  while (WiFi.status() != WL_CONNECTED && attempts < maxAttempts ) {
    
    blinkLED();
    delay(1000);
    Serial.print(".");
    attempts++;
    //ESP.restart(); // Restart if connection fails after 5 seconds
  }
  if (WiFi.status() == WL_CONNECTED){

     Serial.println("✅ Connected to WiFi");
     digitalWrite(BUILTIN_LED, HIGH); // turn on LED
     printWiFiDetails();

     // Optional OTA setup here if not done in main
     ArduinoOTA.setHostname("esp32-ota-test");
     ArduinoOTA.begin();
     Serial.println("📡 OTA Ready");

  } else {
    Serial.println("\n ❌ WiFi Failed after max retries. Check credentials or router.");

    WiFi.softAP("ESP-Setup");
    Serial.print("📶 AP IP address: ");
    Serial.println(WiFi.softAPIP());

    digitalWrite(BUILTIN_LED, LOW);
  }

}

void WiFiManager::printWiFiDetails() {
    Serial.print("📶 IP Address: ");
    Serial.println(WiFi.localIP());
    Serial.print("📶 Signal Strength: ");
    Serial.println(WiFi.RSSI());
    Serial.print("📡 Status: ");
    Serial.println(WiFi.status() == WL_CONNECTED ? "Connected" : "Disconnected");
    Serial.print("📡 SSID: ");
    Serial.println(WiFi.SSID());
}

void WiFiManager::blinkLED() {
  pinMode(BUILTIN_LED, OUTPUT);
  for (int i = 0; i < 3; i++) {
    digitalWrite(BUILTIN_LED, HIGH);
    delay(150);
    digitalWrite(BUILTIN_LED, LOW);
    delay(150);
  }
}