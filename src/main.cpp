#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoOTA.h>
#include "DHT.h"
#include <WebServer.h>

// Calling the WebServer library to handle HTTP requests
WebServer server(80);

// 💡 Pin Definitions
#define LED_R_PIN 18        // GPIO 18 for LED
#define LED_G_PIN 19        // GPIO 19 for LED
#define LED_B_PIN 21        // GPIO 21 for LED
#define DHTPIN 4          // GPIO 22 for DHT data
#define DHTTYPE DHT11     // Change to DHT11 or DHT21 if you're using those


#ifndef WIFI_SSID
#define WIFI_SSID "WhiteSky-Stillwell"  
#endif

#ifndef WIFI_PASSWORD
#define WIFI_PASSWORD "sdhcd2qk"
#endif
// WiFi credentials
const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;
// ----------------------

// 🔧 Initialize DHT sensor
DHT dht(DHTPIN, DHTTYPE);

// ----------------------
// 📡 WiFi Manager Class
// ----------------------
class WiFiManager {
public:
  WiFiManager(const char* ssid, const char* password)
    : ssid(ssid), password(password) {}

  void connect() {
    Serial.println("🔌 Connecting to WiFi...");
    WiFi.begin(ssid, password);

    while (WiFi.waitForConnectResult() != WL_CONNECTED) {
      Serial.println("❌ WiFi Failed! Rebooting in 5 seconds...");
      blinkLED();
      delay(5000);
      ESP.restart();
    }

    Serial.println("✅ Connected to WiFi");
    digitalWrite(BUILTIN_LED, HIGH); // Turn on LED
    printWiFiDetails();
  }

  void printWiFiDetails() {
    Serial.print("📶 IP Address: ");
    Serial.println(WiFi.localIP());
    Serial.print("📶 Signal Strength (RSSI): ");
    Serial.println(WiFi.RSSI());
    Serial.print("📡 Status: ");
    Serial.println(WiFi.status() == WL_CONNECTED ? "Connected" : "Disconnected");
    Serial.print("📡 SSID: ");
    Serial.println(WiFi.SSID());
  }

private:
  const char* ssid;
  const char* password;

  void blinkLED() {
    pinMode(BUILTIN_LED, OUTPUT);
    for (int i = 0; i < 3; i++) {
      digitalWrite(BUILTIN_LED, HIGH);
      delay(300);
      digitalWrite(BUILTIN_LED, LOW);
      delay(300);
    }
  }
};

// 🔧 Global Object
WiFiManager wifiManager(WIFI_SSID, WIFI_PASSWORD);

// Function prototype for htmlPage
String htmlPage(float tempC, float tempF, float humidity);

// ----------------------
// 🔁 Setup Function
// ----------------------
void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(BUILTIN_LED, OUTPUT);
  digitalWrite(BUILTIN_LED, LOW);

  wifiManager.connect();

  dht.begin(); // Start DHT sensor 

  Serial.println("🌡️ DHT Sensor Initialized");
  Serial.print("DHT Sensor Type: ");
  if (DHTTYPE == DHT11) {
    Serial.println("DHT11");
  } else if (DHTTYPE == DHT21) {
    Serial.println("DHT21");
  } else {
    Serial.println("Unknown DHT Sensor Type");
  }

  // Set all the ledSetup bellow 
  ledcSetup(0, 5000, 8); // Channel 0, 5kHz frequency, 8-bit resolution assigned to R LED
  ledcSetup(1,5000, 8); // Channel 1, 5kHz frequency, 8-bit resolution assigned to G LED
  ledcSetup(2, 5000, 8); // Channel 2, 5kHz frequency, 8-bit resolution assigned to B LED
  // PWM setup for LED

  ledcAttachPin(LED_R_PIN, 0); // Attach LED_Red_PIN to channel 0
  ledcAttachPin(LED_G_PIN, 1); // Attach LED_Green_PIN to channel 1
  ledcAttachPin(LED_B_PIN, 2); // Attach LED_Blue_PIN to channel 2

  ArduinoOTA.setHostname("ESP32-OTA-Test");
  ArduinoOTA.begin();

  Serial.println("📡 OTA Ready. Waiting for uploads...");

  // Setup web server route
  server.on("/", HTTP_GET, []() {
    float humidity = dht.readHumidity();
    float temperatureC = dht.readTemperature();
    float temperatureF = dht.readTemperature(true);
    if (isnan(humidity) || isnan(temperatureC)) {
        server.send(500, "text/plain", "⚠️ Failed to read from DHT sensor!");
    } else {
        Serial.print("🌡️ The Tile Temp is: ");
        Serial.print(temperatureF);
        Serial.print(" °F / ");
        Serial.print(temperatureC);
        Serial.println(" °C");

        Serial.print("💧 Humidity levels in your Home: ");
        Serial.print(humidity);
        Serial.println(" %"); 
        String html = htmlPage(temperatureC, temperatureF, humidity);
        server.send(200, "text/html", html);
    }
  });
  server.begin();
  Serial.println("🌐 Web Server started. Access it at http://" + WiFi.localIP().toString() + "/");
}

// Create HTML page for the web server
String htmlPage(float tempC, float tempF, float humidity) {
  String page = "<html><body>";
  page += "<h1>ESP32 DHT Sensor</h1>";
  page += "<p>Temperature: " + String(tempC) + " °C / " + String(tempF) + " °F</p>";
  page += "<p>Humidity: " + String(humidity) + " %</p>";
  page += "</body></html>";
  return page;
}

// ----------------------
// convert RSSI signa strength from dBm to percentage
int rssItoPercantage(int rssi) {
  if (rssi <= -100) {
    return 0; // No signal
  }else if (rssi >= -50) {
    return 100; // Excellent signal
  } else {
    return (rssi + 100) * 100 / 50; // Convert RSSI to percentage
  }
}

// ----------------------
// 🔁 Loop Function
// ----------------------
void loop() {
  ArduinoOTA.handle();

  // variable storing RSSI signal strength
  int rssi = WiFi.RSSI();
  int signalPercentage = rssItoPercantage(rssi); // Convert RSSI to percentage

  static unsigned long lastReadTime = 0;
  unsigned long currentTime = millis();

  if (currentTime - lastReadTime >= 5000) { // every 5 seconds
    lastReadTime = currentTime;

    float humidity = dht.readHumidity();
    float temperatureC = dht.readTemperature();
    float temperatureF = dht.readTemperature(true);

    if (isnan(humidity) || isnan(temperatureC)) {
      Serial.println("⚠️ Failed to read from DHT sensor!");
    } else {
      Serial.print("🌡️ The Tile Temp is: ");
      Serial.print(temperatureF);
      Serial.print(" °F / ");
      Serial.print(temperatureC);
      Serial.println(" °C");

      Serial.print("💧 Humidity levels in your Home: ");
      Serial.print(humidity);
      Serial.println(" %");

      // Map temperature to brightness adjust based on temperature
      int brightness = map(temperatureF, 77, 80, 50, 255); // Map temperature to brightness
      int brightness1 = map(humidity, 60, 70, 50, 255); // Map humidity to brightness
      int brightness2 = map(temperatureC, 30, 35, 50, 255); // Map temperature to brightness

      //// 💡 Adjust LED Brightness Based on Temperature
      brightness = constrain(brightness, 0, 255); // Ensure brightness is within
      brightness1 = constrain(brightness1, 0, 255); // Ensure brightness is within
      brightness2 = constrain(brightness2, 0, 255); // Ensure brightness is within

      ledcWrite(0, brightness); // Write brightness to LED
      ledcWrite(1, brightness1); // Write brightness to Green LED
      ledcWrite(2, brightness2); // Write brightness to Blue LED

      // Print brightness values to Serial Monitor for red LED
      Serial.print("💡 RED LED Brightness: ");
      Serial.println(brightness);
      server.send(200, "text/plain", "Temperature: " + String(temperatureC) + " °C, Humidity: " + String(humidity) + " %");

     // Green LED Brightness based on humidity
      Serial.print("💡 Green LED Brightness: ");
      Serial.print(brightness1);
      Serial.print(" for Humidity: ");
      Serial.print(humidity);
      Serial.println(brightness1);

      // Blue LED Brightness based on temperature
      Serial.print("💡 Blue LED Brightness: ");
      Serial.print(brightness2);
      Serial.print(" for Temperature: ");
      Serial.print(temperatureC);
      Serial.println(brightness2);
      server.send(200, "text/plain", "Temperature: " + String(temperatureC) + " °C, Humidity: " + String(humidity) + " %"); 

      // Print WiFi status
      Serial.print("📶 WiFi Status: ");
      Serial.println(WiFi.status() == WL_CONNECTED ? "Connected" : "Disconnected");
      // Print Signal Strength
      Serial.print("📶 Signal Strength :");
      Serial.println (String(signalPercentage) + " %");

      // Send signal strength to web server
      server.send(200, "text/plain", "Signal Strength: " + String(signalPercentage) + " %");

      // Print IP Address
      Serial.print("📡 IP Address: ");
      Serial.println(WiFi.localIP());

    }
  }
  server.handleClient(); // Handle incoming HTTP requests
  delay(10); // Small delay to avoid blocking
}