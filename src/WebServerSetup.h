// WebServerSetup.h
// This file is part of the ESP32 OTA Test project.

#ifndef WEBSERVER_SETUP_H
#define WEBSERVER_SETUP_H

#include <WebServer.h>

class WebServerSetup {
public:
    WebServerSetup();
    void begin();
    void handleClient();

private:
    WebServer server;
};

#endif
// End of WebServerSetup.h