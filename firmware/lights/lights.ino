#include <TaskScheduler.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>
#include <WiFiManager.h>
#include <Ticker.h>
#include <EEPROM.h>
#include "ws2812.h"
#include "OneButton.h"


#define LOG_ENABLED 1
#define HOSTNAME_PREFIX "tc-robot-semafor-"
#define PIXELS_COUNT 100

String chipId = String(ESP.getChipId(),HEX);
Scheduler scheduler;
