#include <Arduino.h>
#include <SPIFFS.h>
#include <log.h>
#include <SPI.h>
#include <WebServer.h>
#include <TaskScheduler.h>

#include "pinmap.h"
#include "MP3Player.h"
#include "Motors.h"
#include "xwifi.h"
#include <sender.h>
#include "accel.h"
#include "logic.h"
#include "anim.h"

WebServer server;
Motors motors;
MP3Player player;
Scheduler scheduler;

void setup() {
  analogReadResolution(12);
  analogSetWidth(12);
  analogSetAttenuation(ADC_0db);
  delay(1500);

  Serial.begin(115200);
  logInfo("Robot!");

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

  if (!accelSetup(PIN_SDA, PIN_SCL, &scheduler)) {
    abort();
  }

  animSetup(&scheduler);

  if (!logicSetup(&scheduler, &motors, &player)) {
    abort();
  }

  wifiSetup("test");

  senderSetup(&scheduler, logicRecv, NULL);

  logInfo("setup: done");
}

void motorsCb() { motors.tick(); }
Task motorTask(25, TASK_FOREVER, &motorsCb, &scheduler, true);

void batteryCb() {
  static uint32_t vbatSum = 0;
  static uint32_t vbatCount = 0;

  vbatSum += analogRead(PIN_VBAT);
  vbatCount++;

  if (vbatCount < 20)
    return;

  ProtocolMsgVbat msg;
  protocolInit(ProtocolCmdVbat, &msg);
  msg.unitId = logicGetUnitId();
  msg.vbat = (vbatSum / vbatCount) * 4730 / 4095;

  senderSendNow(&msg, sizeof(msg));
  vbatSum = 0;
  vbatCount = 0;
}
Task batteryTask(100, TASK_FOREVER, &batteryCb, &scheduler, true);

void loop() {
  if (wifiHasIp()) {
    animSetMode(AnimModeNormal);
  }

  player.loop();
  scheduler.execute();
  animTick();
  accelTick();

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
  }
}
