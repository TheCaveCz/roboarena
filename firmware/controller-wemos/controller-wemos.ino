#include <TaskScheduler.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>
#include <WiFiManager.h>
#include <Ticker.h>
#include <EEPROM.h>


//#define LOG_ENABLED 1
#define HOSTNAME_PREFIX "tc-roboctrl-"

String chipId = String(ESP.getChipId(),HEX);
Scheduler scheduler;

// this has to be here for prototype injection to work
void meh() {}
