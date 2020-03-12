#include "logic.h"
#include <sender.h>
#include "pinmap.h"
#include <EEPROM.h>
#include "anim.h"
#include "accel.h"

struct Config {
  uint8_t magic;
  uint8_t unitId;
};
#define CONFIG_MAGIC 0x42


static Motors *motors;
static MP3Player *player;
const uint8_t deadZone = 15;
static bool logicBrake;
static Config config;

void logicSetSpeed(uint8_t sx, uint8_t sy) {
  if (sx > (128 - deadZone) && sx < (128 + deadZone))
    sx = 128;
  if (sy > (128 - deadZone) && sy < (128 + deadZone))
    sy = 128;
  int p = sx;

  int q = sy - 128;
  int m1 = constrain(p + q, 0, 255);
  int m2 = constrain(p - q, 0, 255);
  float m1f = m1 / 128.0f - 1.0f;
  float m2f = m2 / 128.0f - 1.0f;

  motors->setSpeed(m1f, m2f);
}

void logicSetBrake(bool brake, bool sound = true) {
  if (brake == logicBrake)
    return;
  logValue("logic: brake = ", brake);
  logicBrake = brake;

  motors->setBrake(brake);
  animSetMode(brake ? AnimModeBrake : AnimModeNormal);

  if (sound) {
    player->play(brake ? "/dead-2.mp3" : "/start-1.mp3");
  }
}

void logicRecv(ProtocolCmd cmd, void *buffer, size_t len) {
  if (cmd == ProtocolCmdMove && logicValidUnitId()) {
    ProtocolMsgMove *msg = (ProtocolMsgMove *)buffer;
    uint8_t index = logicGetUnitId() - 1;
    logicSetSpeed(msg->speeds[index * 2], msg->speeds[index * 2 + 1]);
    logicSetBrake(msg->brakeMask & _BV(index) ? true : false);

  } else if (cmd == ProtocolCmdDiscoverRequest) {
    ProtocolMsgDiscoverResponse response;
    protocolInit(ProtocolCmdDiscoverResponse, &response);
    response.unitId = logicGetUnitId();
    response.fwVersion = FW_VERSION;
    esp_efuse_mac_get_default(response.mac);
    senderSendNow(&response, sizeof(response), senderGetIp(), senderGetPort());

  } else if (cmd == ProtocolCmdSetId) {
    ProtocolMsgSetId *msg = (ProtocolMsgSetId *)buffer;
    logicSetUnitId(msg->unitId);
  }
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
  animSetHighlight(a);
}
Task logicAccelTask(25, TASK_FOREVER, &logicAccelCb);

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

  return true;
}
