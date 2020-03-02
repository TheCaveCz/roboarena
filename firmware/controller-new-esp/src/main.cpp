#include <Arduino.h>
#include <TaskScheduler.h>
#include <ArduinoOTA.h>
#include <Ticker.h>
#include <log.h>
#include "wifi.h"
#include "control.h"
#include "sender.h"

#define HOSTNAME_PREFIX "tc-roboctrl-"

String ssid;
Scheduler scheduler;
Ticker animTicker;
uint8_t animState;


void senderReceive(ProtocolCmd cmd, void *buffer, size_t len) {}

size_t senderSend(uint8_t id, void *buffer) {
  if (id == 0) {
    ProtocolMsgMove msg;
    protocolInit(ProtocolCmdMove, &msg);
    memcpy(msg.speeds, controlGetPos(), sizeof(msg.speeds));
    msg.brakeMask = controlGetBrakeMask();
    memcpy(buffer, &msg, sizeof(msg));
    return sizeof(msg);
  } else {
    return 0;
  }
}

void buttonCallback(uint8_t buttonId) {
  if (buttonId == 6) {
    wifiResetAndRestart();
  }
}

void animTickerCb() {
  switch (animState) {
    case 0:
      Serial.print("Abcd");
      animState++;
      break;
    case 1:
      Serial.print("aBcd");
      animState++;
      break;
    case 2:
      Serial.print("abCd");
      animState++;
      break;
    case 3:
      Serial.print("abcD");
      animState = 0;
      break;

    case 10:
      Serial.write(controlGetBrakeMask() & 1 ? 'a' : 'A');
      Serial.write(controlGetBrakeMask() & 2 ? 'b' : 'B');
      Serial.write(controlGetBrakeMask() & 4 ? 'c' : 'C');
      Serial.write(controlGetBrakeMask() & 8 ? 'd' : 'D');
      animState = 11;
      break;
    case 11:
      Serial.print("ABCD");
      animState = 10;
      break;

    default:
      Serial.print("abcd");
      animState = 10;
      break;
  }
}

void setup() {
  ssid = HOSTNAME_PREFIX;
  ssid += String(ESP.getChipId(), HEX);

  delay(2000);

  Serial.begin(57600);
  Serial.print("rabcd");
  logInfo("Roboarena ctrl!!!");

  controlSetup(&Serial, &scheduler, buttonCallback);

  animState = 0;
  animTickerCb();
  animTicker.attach(0.25, &animTickerCb);
  wifiSetup(ssid);

  ArduinoOTA.setHostname(ssid.c_str());
#ifdef OTA_PASSWORD
  ArduinoOTA.setPassword(OTA_PASSWORD);
#endif
  ArduinoOTA.onStart([]() { logInfo("Starting OTA"); });
  ArduinoOTA.begin();

  senderSetup(&scheduler, senderReceive, senderSend);
}

void loop() {
  ArduinoOTA.handle();
  scheduler.execute();

  if (wifiHasIp()) {
    animState = 10;
    Serial.print("R");
  }
}
