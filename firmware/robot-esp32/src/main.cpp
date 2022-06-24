#include <Arduino.h>
#include <SPIFFS.h>
#include <log.h>
#include <SPI.h>
#include <TaskScheduler.h>

#include "pinmap.h"
#include "MP3Player.h"
#include "Motors.h"
#include <sender.h>
#include "accel.h"
#include "logic.h"
#include "anim.h"

#if USE_WIFI
#include <ArduinoOTA.h>
#include "xwifi.h"
#endif
Motors motors;
MP3Player player;
Scheduler scheduler;

void setup() {
  animSetup(&scheduler);

  analogReadResolution(12);
  analogSetWidth(12);
  analogSetAttenuation(ADC_0db);
  delay(1500);

  Serial.begin(115200);
  logInfo("Robot!");

  uint8_t mac[6];
  esp_efuse_mac_get_default(mac);
  uint32_t chipIdInt = mac[3] << 16 | mac[4] << 8 | mac[5];
  logValueEx("chipId: ", chipIdInt, HEX);

  if (!SPIFFS.begin()) {
    logInfo("spiffs: failed to begin");
    abort();
  }

  if (!player.begin(PIN_I2S_BLCK, PIN_I2S_LRCLK, PIN_I2S_DIN)) {
    abort();
  }

  if (!motors.begin()) {
    abort();
  }

  accelSetup(PIN_SDA, PIN_SCL, &scheduler);

  if (!logicSetup(&scheduler, &motors, &player)) {
    abort();
  }

#if USE_WIFI
  animSetMode(AnimModeConnecting);
  wifiSetup("test");
  WiFi.setSleep(false);

  String host = "robot-";
  host += String(chipIdInt, HEX);
  ArduinoOTA.setHostname(host.c_str());
  ArduinoOTA
      .onStart([]() {
        logicSetBrake(true, false);
        String type;
        if (ArduinoOTA.getCommand() == U_FLASH) {
          logInfo("ota: start sketch update");
        } else { // U_SPIFFS
          SPIFFS.end();
          logInfo("ota: start SPIFFS update");
        }
      })
      .onEnd([]() { logInfo("ota: finished"); })
      .onProgress([](unsigned int progress, unsigned int total) {
        static uint32_t otaProgress = 0;
        uint32_t p = (progress / (total / 100));
        if (p != otaProgress) {
          otaProgress = p;
          logValue("ota: progress: ", otaProgress);
        }
      })
      .onError([](ota_error_t error) { logValue("ota: error ", error); });

  ArduinoOTA.begin();
#else
  animSetMode(AnimModeNormal);
#endif
  senderSetup(&scheduler, logicRecv, NULL);

  logInfo("setup: done");
}

void motorsCb() { motors.tick(); }
Task motorTask(25, TASK_FOREVER, &motorsCb, &scheduler, true);

void batteryCb() {
  ProtocolMsgVbat msg;
  protocolInit(ProtocolCmdVbat, &msg);
  msg.unitId = logicGetUnitId();
  msg.vbat = logicGetVbat();
  if (msg.vbat == 0)
    return;

  senderSendNow(&msg, sizeof(msg));
}
Task batteryTask(1000, TASK_FOREVER, &batteryCb, &scheduler, true);

void loop() {
#if USE_WIFI
  if (wifiHasIp()) {
    animSetMode(AnimModeNormal);
  }

  ArduinoOTA.handle();
#endif
  senderTick();
  player.loop();
  scheduler.execute();
  animTick();
  accelTick();
  /*
    if (Serial.available()) {
      String line = Serial.readStringUntil('\n');
      line.trim();
      if (line.startsWith("p")) {
        line.remove(0, 1);
        motors.setP(line.toInt() / 1000.0f);
      }
      if (line.startsWith("i")) {
        line.remove(0, 1);
        motors.setI(line.toInt() / 1000.0f);
      }
      if (line.startsWith("d")) {
        line.remove(0, 1);
        motors.setD(line.toInt() / 1000.0f);
      }
      if (line.startsWith("f")) {
        line.remove(0, 1);
        motors.setF(line.toInt() / 1000.0f);
      }
      if (line.startsWith("m")) {
        line.remove(0, 1);
        motors.setMaxSpeed(line.toInt() / 100.0f);
      }
      if (line.startsWith("?")) {
        motors.dumpPid();
      }
      if (line.startsWith("u")) {
        line.remove(0, 1);
        logicSetUnitId(line.toInt());
      }
      if (line.startsWith("c")) {
        line.remove(0, 1);
        logicRunVbatCalib(line.toInt() != 0);
      }
    }*/
}
