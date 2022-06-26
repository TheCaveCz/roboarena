#include "logic.h"
#include <sender.h>
#include "pinmap.h"
#include <EEPROM.h>
#include "anim.h"
#include "accel.h"

struct Config {
  uint8_t magic;
  uint8_t unitId;
  uint16_t vbatCalib;
};
#define CONFIG_MAGIC 0x42


static Motors *motors;
static MP3Player *player;
const uint8_t deadZone = 15;
static bool logicBrake;
static bool logicBoost;
static Config config;
static uint32_t vbat;

void logicSetSpeed(uint8_t sx, uint8_t sy) {
  int p = sx;
  int q = sy - 128;
  int m1 = constrain(p + q, 0, 255);
  int m2 = constrain(p - q, 0, 255);
  float m1f = m1 / 128.0f - 1.0f;
  float m2f = m2 / 128.0f - 1.0f;

  motors->setSpeed(m1f, m2f);
}

void logicUpdateAnim() { animSetMode(logicBrake ? AnimModeBrake : (logicBoost ? AnimModeBoost : AnimModeNormal)); }

void logicSetBrake(bool brake, bool sound) {
  if (brake == logicBrake)
    return;
  logValue("logic: brake = ", brake);
  logicBrake = brake;

  motors->setBrake(brake);
  logicUpdateAnim();

  if (sound) {
    player->play(brake ? "/dead-2.mp3" : "/start-2.mp3");
  }
}

void logicSetBoost(bool boost) {
  if (boost == logicBoost)
    return;
  logValue("logic: boost=", boost);
  logicBoost = boost;

  motors->setMaxSpeed(boost ? 0.75 : 0.5);

  logicUpdateAnim();
  if (boost && !player->isPlaying()) {
    player->play("/dead-3.mp3");
  }
}

void logicRecv(ProtocolCmd cmd, const void *buffer, size_t len) {
  if (cmd == ProtocolCmdMove && logicValidUnitId()) {
    const ProtocolMsgMove *msg = (const ProtocolMsgMove *)buffer;
    uint8_t index = logicGetUnitId() - 1;
    logicSetSpeed(msg->speeds[index * 2], msg->speeds[index * 2 + 1]);
    logicSetBrake(msg->brakeMask & _BV(index) ? true : false);
    logicSetBoost(msg->brakeMask & 0x80 ? true : false);

  } else if (cmd == ProtocolCmdDiscoverRequest) {
    ProtocolMsgDiscoverResponse response;
    protocolInit(ProtocolCmdDiscoverResponse, &response);
    response.unitId = logicGetUnitId();
    response.fwVersion = FW_VERSION;
    esp_efuse_mac_get_default(response.mac);
    senderSendNow(&response, sizeof(response));

  } /* else if (cmd == ProtocolCmdSetId) {
    ProtocolMsgSetId *msg = (ProtocolMsgSetId *)buffer;
    logicSetUnitId(msg->unitId);
  }*/
}

void logicUpdateAnimColor() {
  switch (logicGetUnitId()) {
    case 1:
      animSetColor(AnimColorRed);
      break;
    case 2:
      animSetColor(AnimColorBlue);
      break;
    case 3:
      animSetColor(AnimColorYellow);
      break;
    case 4:
      animSetColor(AnimColorGreen);
      break;
    default:
      animSetColor(AnimColorUnknown);
      break;
  }
}

void logicSaveConfig() {
  config.magic = CONFIG_MAGIC;
  EEPROM.put(0, config);
  EEPROM.commit();
}

bool logicValidUnitId() { return config.unitId >= 1 && config.unitId <= 4; }

uint8_t logicGetUnitId() { return config.unitId; }

void logicSetUnitId(uint8_t newId) {
  if (newId < 1 || newId > 4)
    return;

  logValue("logic: setting unit id to ", newId);

  config.unitId = newId;
  logicSaveConfig();
  logicUpdateAnimColor();
};

void logicAccelCb() {
  float a = accelGet();
  if (a > 0.6 && !player->isPlaying()) {
    player->play("/hit-1.mp3");
  }
  animSetHighlight(a);
}
Task logicAccelTask(25, TASK_FOREVER, &logicAccelCb);


static uint32_t calibSum = 0;
static uint32_t calibCount = 0;

void logicBatMeasureCb() {
  static uint32_t vbatSum = 0;
  static uint32_t vbatCount = 0;

  vbatSum += analogRead(PIN_VBAT);
  vbatCount++;

  if (vbatCount >= 200) {
    uint32_t vbatRaw = vbatSum / vbatCount;
    calibSum += 3700 * 4095 / vbatRaw;
    calibCount++;

    vbat = vbatRaw * config.vbatCalib / 4095;
    vbatSum = 0;
    vbatCount = 0;

    // Serial.printf("%d %d %d\n", vbatRaw, calibSum / calibCount, vbat);
  }
}
Task logicBatMeasureTask(5, TASK_FOREVER, &logicBatMeasureCb);

uint32_t logicGetVbat() { return vbat; }

void logicRunVbatCalib(bool on) {
  if (on) {
    calibCount = 0;
    calibSum = 0;
  } else if (calibCount) {
    config.vbatCalib = calibSum / calibCount;
    logicSaveConfig();
  }
}

bool logicSetup(Scheduler *scheduler, Motors *m, MP3Player *p) {
  motors = m;
  player = p;

  if (!EEPROM.begin(sizeof(config))) {
    logInfo("logic: failed to open config");
    return false;
  }
  EEPROM.get(0, config);
  if (config.magic != CONFIG_MAGIC) {
    memset(&config, 0, sizeof(config));
    logicSaveConfig();
  }
  logicUpdateAnimColor();
  logValue("logic: unitId = ", config.unitId);

  scheduler->addTask(logicAccelTask);
  logicAccelTask.enable();
  scheduler->addTask(logicBatMeasureTask);
  logicBatMeasureTask.enable();

  return true;
}
