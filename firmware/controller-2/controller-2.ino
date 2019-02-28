#include <TaskScheduler.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>
#include <WiFiManager.h>
#include <Ticker.h>

#define LOG_ENABLED 1

#define HOSTNAME_PREFIX "tc-roboctrl2-"

String chipId = String(ESP.getChipId(),HEX);
Scheduler scheduler;
