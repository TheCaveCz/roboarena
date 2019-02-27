#include <TaskScheduler.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>
#include <WiFiManager.h>
#include <Ticker.h>
#include <EEPROM.h>
#include "ws2812.h"
#include "OneButton.h"

#define PIN_M1_A D2
#define PIN_M1_B D1
#define PIN_M1_EN D7

#define PIN_M2_A D0
#define PIN_M2_B D6
#define PIN_M2_EN D5

#define LOG_ENABLED 1
#define HOSTNAME_PREFIX "tc-robot-"
#define PIXELS_COUNT 3

#define FW_VERSION 2

String chipId = String(ESP.getChipId(),HEX);
Scheduler scheduler;
