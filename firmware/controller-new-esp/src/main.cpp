#include <Arduino.h>
#include <TaskScheduler.h>
#include <Ticker.h>
#include <log.h>
#include "control.h"
#include <sender.h>
#include "anim.h"
#include "logic.h"
#include "disp.h"

#if USE_WIFI
#include <ArduinoOTA.h>
#include <ESP8266WebServer.h>
#include "wifi.h"
#endif

#if LOGIC_CONTROLLER
#define HOSTNAME_PREFIX "tc-roboctrl-"
#elif LOGIC_REMOTE
#define HOSTNAME_PREFIX "tc-roboremote-"
#else
#error Unknown config
#endif

String ssid;
Scheduler scheduler;

#if USE_WIFI
ESP8266WebServer server;

void serverHandleRoot() {
  String response = "vbat=";
  response += controlGetVbat();
#if LOGIC_CONTROLLER
  response += "\ntype=controller";
#elif LOGIC_REMOTE
  response += "\ntype=remote";
#else
#error unknown
#endif
  response += "\nuptime=";
  response += millis();
  response += "\nid=";
  response += ssid;

  server.send(200, "text/plain", response);
}
#endif

void reportingCb() {
#if LOGIC_CONTROLLER
  Serial.print("R"); // enable controller positions reporting
#endif
#if LOGIC_REMOTE
  Serial.print("r"); // disable controller positions reporting
#endif
}

Task reportingTask(5000, TASK_FOREVER, &reportingCb, &scheduler);

void setup() {
  ssid = HOSTNAME_PREFIX;
  ssid += String(ESP.getChipId(), HEX);

  delay(2000);

  Serial.begin(57600);
  Serial.print("rabcd"); // turn off all leds and disable controller reporting
  logInfo("Roboarena ctrl!!!");

  controlSetup(&Serial, &scheduler, buttonCallback);

  animSetup();

#if USE_WIFI
  wifiSetup(ssid);
  WiFi.setSleepMode(WIFI_NONE_SLEEP);

  server.on("/", serverHandleRoot);
  server.begin();

  ArduinoOTA.setHostname(ssid.c_str());
#ifdef OTA_PASSWORD
  ArduinoOTA.setPassword(OTA_PASSWORD);
#endif
  ArduinoOTA.onStart([]() { logInfo("Starting OTA"); });
  ArduinoOTA.begin();
#endif
  senderSetup(&scheduler, senderReceive, senderSend);

  logicSetup(&scheduler);
  dispSetup(&scheduler);

#if !USE_WIFI
  animSetState(AnimBlinkBrake);
  reportingTask.enable();
#endif
}

void loop() {
#if USE_WIFI
  ArduinoOTA.handle();
  server.handleClient();
#endif
  senderTick();
  scheduler.execute();

#if USE_WIFI
  if (wifiHasIp()) {
    animSetState(AnimBlinkBrake);
    reportingTask.enable();
  }
#endif
}
