#include <Arduino.h>
#include <TaskScheduler.h>
#include <ArduinoOTA.h>
#include <Ticker.h>
#include <log.h>
#include "wifi.h"
#include "control.h"
#include <sender.h>
#include "anim.h"
#include "logic.h"

#if LOGIC_CONTROLLER
#define HOSTNAME_PREFIX "tc-roboctrl-"
#elif LOGIC_REMOTE
#define HOSTNAME_PREFIX "tc-roboremote-"
#else
#error Unknown config
#endif

String ssid;
Scheduler scheduler;

void setup() {
  ssid = HOSTNAME_PREFIX;
  ssid += String(ESP.getChipId(), HEX);

  delay(2000);

  Serial.begin(57600);
  Serial.print("rabcd"); // turn off all leds and disable controller reporting
  logInfo("Roboarena ctrl!!!");

  controlSetup(&Serial, &scheduler, buttonCallback);

  animSetup();
  wifiSetup(ssid);

  ArduinoOTA.setHostname(ssid.c_str());
#ifdef OTA_PASSWORD
  ArduinoOTA.setPassword(OTA_PASSWORD);
#endif
  ArduinoOTA.onStart([]() { logInfo("Starting OTA"); });
  ArduinoOTA.begin();

  senderSetup(&scheduler, senderReceive, senderSend);
}

void reportingCb() {
#if LOGIC_CONTROLLER
  Serial.print("R"); // enable controller positions reporting
#endif
#if LOGIC_REMOTE
  Serial.print("r"); // disable controller positions reporting
#endif
}

Task reportingTask(5000, TASK_FOREVER, &reportingCb, &scheduler);

void loop() {
  ArduinoOTA.handle();
  scheduler.execute();

  if (wifiHasIp()) {
    animSetState(AnimBlinkBrake);
    reportingTask.enable();
  }
}
