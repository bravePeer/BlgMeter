#pragma once
#include <ESP8266WebServer.h>

extern ESP8266WebServer server;

void test_handler()
{
    server.args();

}