// WiFiManager.h
// This file is part of the ESP32 OTA Test project.

#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <WiFi.h>

class WiFiManager {
public:
  WiFiManager(const char* ssid, const char* password);
  void connect();
  void printWiFiDetails();

private:
  const char* ssid;
  const char* password;
  void blinkLED();
};

#endif