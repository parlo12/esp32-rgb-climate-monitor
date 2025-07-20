#ifndef TELNET_DEBUG_H
#define TELNET_DEBUG_H

#include <WiFi.h>
#include <WiFiClient.h>

WiFiServer telnetServer(23);
WiFiClient telnetClient;

// set up telnet

void setupTelnet() {
    telnetServer.begin();
    telnetServer.setNoDelay(true);

    Serial.println("❓ Telnet server started on port 23");
}

// hanlde function for 

void handleTelnet() {
    if(telnetServer.hasClient()){
        if(!telnetClient || !telnetClient.connected());
        telnetClient = telnetServer.available();
        Serial.println("🧑‍💻 Telnet client connected");
        telnetClient.println('✅ Telenet Debug Connected');
    } else {
        WiFiClient rejectClient = telnetServer.available();
        rejectClient.println("❌ Only one client allowed");
        rejectClient.stop();
    }
    if (telnetClient && telnetClient.connected()) {
        if(telnetClient.available()) {
            String command = telnetClient.readStringUntil('\n');
            command.trim();
            telnetClient.print("You said: ");
            telnetClient.println(command);
        }
    }
}

#define TPRINT(x)        \
  do {                   \
    Serial.print(x);     \
    if (telnetClient) telnetClient.print(x); \
  } while (0)

#define TPRINTLN(x)      \
  do {                   \
    Serial.println(x);   \
    if (telnetClient) telnetClient.println(x); \
  } while (0)

#endif
