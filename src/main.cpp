// main.cpp
// This file is part of the ESP32 OTA Test project.
#include <Arduino.h>
#include <DHT.h>
#include <WebServer.h>
#include <ArduinoOTA.h>
#include "WiFiManager.h"
#include "LEDControl.h"
#include "SensorHandler.h"
#include "config.h"
#include "TelnetDebug.h"

// 🔌 Pin and sensor setup
#define LED_R_PIN 18
#define LED_G_PIN 19
#define LED_B_PIN 21
#define DHTPIN 4
#define DHTTYPE DHT11

#ifndef WIFI_SSID
#define WIFI_SSID "WhiteSky-Stillwell"
#endif

#ifndef WIFI_PASSWORD
#define WIFI_PASSWORD "sdhcd2qk"
#endif

WebServer server(80);
// DHT dht(DHTPIN, DHTTYPE);
WiFiManager wifiManager(WIFI_SSID, WIFI_PASSWORD);
LEDControl ledController(LED_R_PIN, LED_G_PIN, LED_B_PIN);
SensorHandler sensorHandler(DHTPIN, DHTTYPE);

// Convert RSSI (signal strength) to percentage
int rssiToPercentage(int rssi) {
  if (rssi <= -100) return 0;
  if (rssi >= -50) return 100;
  return (rssi + 100) * 100 / 50;
}

// Web page content
String htmlPage(float tempC, float tempF, float humidity) {
  int red = map(tempF, 60, 80, 50, 255);
  int green = map(humidity, 60, 80, 50, 255);
  int blue = map(tempC, 60, 80, 50, 255);

  red = constrain(red, 0, 255);
  green = constrain(green, 0, 255);
  blue = constrain(blue, 0, 255);

  String page = R"rawliteral(
  <!DOCTYPE html>
  <html>
  <head>
    <meta charset="UTF-8">
    <title>ESP32 Home Dashboard</title>
    <style>
      body {
        margin: 0;
        font-family: apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, Oxygen;
        background: linear-gradient(to top, #1e3c72, #2a5298);
        color: white;
        text-align: center;
      }
      .card {
        background: rgba(255, 255, 255, 0.1);
        border.radius: 20px;
        padding: 15px;
        backdrop-filter: bluer(10px);
        box-shadow: 0 4px 10px rgba(0,0,0,0,3);
        display: inline-bock;
        width: 280px;
      }

      .label {
        font-size: 18px;
        color: #ccc;
      }

      .value {
        font-size: 36px;
        font-weight: 600;
        margin-top: 5px;
      }

      .led-bar {
        height: 15px;
        width: 100%;
        background: #333;
        border-radius: 10px;
        overflow: hidden:
        margin-top: 10px;
      }

      .led-fill {
        height: 100%;
      }

      .led-r {background: red;}
      .led-r {background: lime;}
      .led-r {background: cyan;}
      
    </style>
  </head>
  <body>
    <h2>🏠 ESP32 Home Dashboard</h2>
    <div class="card"> 
      <div class="label">🌡 Temperature</div>
      <div class="value">
  )rawliteral";
   page += String(tempF, 1) + " °F / " + String(tempC, 1) + " °C</div></div>";
    page += R"rawliteral(
    <div class="card">
      <div class="label">💧 Humidity</div>
      <div class="value">)rawliteral";
    page += String(humidity, 1) + " %</div></div>";

    page += R"rawliteral(
    <div class="card">
      <div class="label">🔆 LED Brightness</div>
      <div class="led-bar"><div class="led-fill led-r" style="width:)rawliteral";
    page += String((red / 255.0) * 100) + R"rawliteral(%"></div></div>
      <div class="led-bar"><div class="led-fill led-g" style="width:)rawliteral";
    page += String((green / 255.0) * 100) + R"rawliteral(%"></div></div>
      <div class="led-bar"><div class="led-fill led-b" style="width:)rawliteral";
    page += String((blue / 255.0) * 100) + R"rawliteral(%"></div></div>

    </div>
  </body>
  </html>
  )rawliteral";

  return page;
}

void setup() {
  Serial.begin(115200);
  pinMode(BUILTIN_LED, OUTPUT);
  digitalWrite(BUILTIN_LED, LOW);

  Serial.println("MAC Address : " + WiFi.macAddress());
  Serial.println("Firmware Version : " + String(ESP.getSdkVersion()));
  Serial.print("IP Address : " );
  Serial.println(WiFi.localIP());

  wifiManager.connect();
  setupTelnet();

  Serial.println("🌐 WiFi Connected");
  ArduinoOTA.setHostname("esp32-ota-test");
  ArduinoOTA.begin();
  Serial.println("📡 OTA Ready");

  // Telnet Debug logs

  TPRINT("🔧 Setup started");
  TPRINT("📡 ESP32 IP Address");
  TPRINTLN(WiFi.localIP());
  TPRINTLN("📡 OTA Ready");

  sensorHandler.begin();
  ledController.begin();

    // Setup web server route
  server.on("/", HTTP_GET, []() {
    float humidity = sensorHandler.getHumidity();
    float tempC = sensorHandler.getTemperatureC();
    float tempF = sensorHandler.getTemperatureF();

    if (isnan(humidity) || isnan(tempC)) {
      server.send(500, "text/plain", "⚠️ Failed to read from DHT sensor!");
    } else {
      server.send(200, "text/html", htmlPage(tempC, tempF, humidity));
    }
  });

  server.begin();
  Serial.println("🌐 Web Server started at http://" + WiFi.localIP().toString());
}

void loop() {
  ArduinoOTA.handle();
  server.handleClient();
  handleTelnet();

  static unsigned long lastRead = 0;
  if (millis() - lastRead > 5000) {
    lastRead = millis();

    float humidity = sensorHandler.getHumidity();
    float tempC = sensorHandler.getTemperatureC();
    float tempF = sensorHandler.getTemperatureF();

    if (isnan(humidity) || isnan(tempC)) {
      Serial.println("⚠️ Failed to read from DHT sensor!");
      return;
    }

    int rssi = WiFi.RSSI();
    int signalPercent = rssiToPercentage(rssi);

    Serial.printf("🌡️ %.2f °F / %.2f °C, 💧 %.2f %%\n", tempF, tempC, humidity);
    Serial.print("📶 Signal: ");
    Serial.println(String(signalPercent) + " %");

    // Print IP address data
    Serial.print("IP Address :");
    Serial.println(WiFi.localIP());

    // Brightness calculations
    int redBrightness = map(tempF, 77, 80, 50, 255);
    int greenBrightness = map(humidity, 50, 77, 50, 255);
    int blueBrightness = map(tempC, 30, 35, 50, 255);

    ledController.update(
      constrain(redBrightness, 0, 255),
      constrain(greenBrightness, 0, 255),
      constrain(blueBrightness, 0, 255)
    );

    Serial.println("LED Red Brightness level :");
    Serial.println(redBrightness);

    Serial.println("LED Green Brightness level :");
    Serial.println(greenBrightness);

    Serial.print("LED Blue Brightness level :");
    Serial.println(blueBrightness);

    //Telnet Printing to remote terminal

    TPRINT("🌡️ Temp: ");
    TPRINT(sensorHandler.getTemperatureC());
    TPRINT(" °C / ");
    TPRINT(sensorHandler.getTemperatureF());
    TPRINTLN(" °F");

    TPRINT("💧 Humidity: ");
    TPRINTLN(sensorHandler.getHumidity());

    TPRINT("📶 WiFi Signal Strenght: ");
    TPRINT( String(signalPercent));
    TPRINTLN(" %");

    // LED Brightness
    TPRINT(" LED red brightness :");
    TPRINTLN(redBrightness);


  }

  delay(10);
}