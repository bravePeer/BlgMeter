#pragma once
#include <ESP8266WebServer.h>

extern ESP8266WebServer server;

namespace webHandle
{
    void index()
    {
        Serial.println("Client connected");
        server.send_P(200,"text/html", configureSite);
    }

    void test_handle()
    {
        server.args();
        server.send_P(200, "text/html", "yoł");
    }
}