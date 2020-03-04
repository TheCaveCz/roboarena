#if LOGIC_CONTROLLER
#include "logic.h"
#include "control.h"
#include "wifi.h"
#include <sender.h>

void toggleBrakeBit(uint8_t bit) {
  uint8_t bm = controlGetBrakeMask();
  if (bm & (1 << bit)) {
    bm &= ~(1 << bit);
  } else {
    bm |= (1 << bit);
  }
  controlSetBrakeMask(bm);
}

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
        break;
      case ProtocolRemoteCommandCalibrate:
        Serial.write('X');
        break;
    }
  }
}

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
Task batteryTask(5000, TASK_FOREVER, &batteryCb);

void logicSetup(Scheduler *scheduler) {
  scheduler->addTask(batteryTask);
  batteryTask.enable();
}

#endif