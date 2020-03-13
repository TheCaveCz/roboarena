#if LOGIC_CONTROLLER
#include "logic.h"
#include "control.h"
#include "wifi.h"
#include <sender.h>
#include "anim.h"

void toggleBrakeBit(uint8_t bit) {
  uint8_t bm = controlGetBrakeMask();
  if (bm & (1 << bit)) {
    bm &= ~(1 << bit);
  } else {
    bm |= (1 << bit);
  }
  controlSetBrakeMask(bm);
}

uint32_t lastMessage[5];
bool boost = false;

uint8_t vbatIndex(uint8_t unitId) {
  switch (unitId) {
    case 1:
      return 0;
    case 2:
      return 1;
    case 3:
      return 2;
    case 4:
      return 3;
    case 255:
      return 4;
    default:
      return 255;
  }
}

bool isPresent(uint8_t unitId) {
  uint8_t idx = vbatIndex(unitId);
  if (idx == 255)
    return false;
  return millis() - lastMessage[idx] < 5000;
}

void updatePresence() { animSetPresence(isPresent(1) | isPresent(2) << 1 | isPresent(3) << 2 | isPresent(4) << 3); }

void senderReceive(ProtocolCmd cmd, void *buffer, size_t len) {
  if (cmd == ProtocolCmdRemoteCtrl) {
    ProtocolMsgRemoteCtrl *msg = (ProtocolMsgRemoteCtrl *)buffer;
    switch (msg->command) {
      case ProtocolRemoteCommandToggleBrake1:
        toggleBrakeBit(0);
        break;
      case ProtocolRemoteCommandToggleBrake2:
        toggleBrakeBit(1);
        break;
      case ProtocolRemoteCommandToggleBrake3:
        toggleBrakeBit(2);
        break;
      case ProtocolRemoteCommandToggleBrake4:
        toggleBrakeBit(3);
        break;
      case ProtocolRemoteCommandStartAll:
        controlSetBrakeMask(0);
        break;
      case ProtocolRemoteCommandStopAll:
        controlSetBrakeMask(0xf);
        boost = false;
        break;
      case ProtocolRemoteCommandCalibrate:
        Serial.write('X');
        break;
      case ProtocolRemoteCommandBoostStart:
        boost = true;
        break;
      case ProtocolRemoteCommandBoostEnd:
        boost = false;
        break;
    }
  } else if (cmd == ProtocolCmdVbat) {
    ProtocolMsgVbat *msg = (ProtocolMsgVbat *)buffer;
    uint8_t idx = vbatIndex(msg->unitId);
    if (idx != 255) {
      lastMessage[idx] = millis();
      updatePresence();
    }
  }
}

size_t senderSend(uint8_t id, void *buffer) {
  if (id == 0) {
    ProtocolMsgMove msg;
    protocolInit(ProtocolCmdMove, &msg);
    memcpy(msg.speeds, controlGetPos(), sizeof(msg.speeds));
    msg.brakeMask = controlGetBrakeMask();
    if (boost)
      msg.brakeMask |= 0x80;
    memcpy(buffer, &msg, sizeof(msg));
    return sizeof(msg);
  } else {
    return 0;
  }
}

void buttonCallback(uint8_t buttonId, ButtonEventType et) {
  if (buttonId == 6 && et == EventPress) {
    wifiResetAndRestart();
  }
}

void batteryCb() {
  ProtocolMsgVbat msg;
  protocolInit(ProtocolCmdVbat, &msg);
  msg.unitId = 255;
  msg.vbat = controlGetVbat();
  senderSendNow(&msg, sizeof(msg));
}
Task batteryTask(1000, TASK_FOREVER, &batteryCb);

void logicSetup(Scheduler *scheduler) {
  scheduler->addTask(batteryTask);
  batteryTask.enable();
}

#endif