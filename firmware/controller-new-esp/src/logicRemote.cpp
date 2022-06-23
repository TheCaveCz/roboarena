#if LOGIC_REMOTE
#include "logic.h"
#include <log.h>
#include <RoboProtocol.h>
#include <sender.h>
#include "control.h"
#include <TaskSchedulerDeclarations.h>
#include <ssd1306.h>
#include <nano_engine.h>
#include "anim.h"
#if USE_WIFI
#include "wifi.h"
#endif

uint8_t speeds[8];
uint16_t vbats[5];
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

void senderReceive(ProtocolCmd cmd, const void *buffer, size_t len) {
  if (cmd == ProtocolCmdMove) {
    ProtocolMsgMove *msg = (ProtocolMsgMove *)buffer;
    controlSetBrakeMask(msg->brakeMask);
    boost = (msg->brakeMask & 0x80) != 0;
    memcpy(speeds, msg->speeds, sizeof(speeds));

  } else if (cmd == ProtocolCmdVbat) {
    ProtocolMsgVbat *msg = (ProtocolMsgVbat *)buffer;
    uint8_t idx = vbatIndex(msg->unitId);
    if (idx != 255) {
      vbats[idx] = msg->vbat;
      lastMessage[idx] = millis();
    }
    logValue2("vbat info from ", msg->unitId, "vbat ", msg->vbat);
  }
}

size_t senderSend(uint8_t id, void *buffer) { return 0; }

void buttonCallback(uint8_t buttonId, ButtonEventType et) {
  ProtocolMsgRemoteCtrl msg;
  protocolInit(ProtocolCmdRemoteCtrl, &msg);
  if (et == EventLongStart) {
    switch (buttonId) {
      case 1:
        msg.command = ProtocolRemoteCommandBoostStart;
        break;
      case 2:
        msg.command = ProtocolRemoteCommandBoostEnd;
        break;
      case 5:
        msg.command = ProtocolRemoteCommandCalibrate;
        break;
#if USE_WIFI
      case 6:
        wifiResetAndRestart();
        return;
#endif
      default:
        return;
    }

  } else if (et == EventPress) {
    switch (buttonId) {
      case 1:
        msg.command = ProtocolRemoteCommandToggleBrake1;
        break;
      case 2:
        msg.command = ProtocolRemoteCommandToggleBrake2;
        break;
      case 3:
        msg.command = ProtocolRemoteCommandToggleBrake3;
        break;
      case 4:
        msg.command = ProtocolRemoteCommandToggleBrake4;
        break;
      case 5:
        msg.command = ProtocolRemoteCommandStartAll;
        break;
      case 6:
        msg.command = ProtocolRemoteCommandStopAll;
        break;
    }
  } else {
    return;
  }
  senderSendNow(&msg, sizeof(msg));
}

uint8_t framebuffer[128 * 64 / 8];
class Canvas : public NanoCanvas1 {
public:
  using NanoCanvas1::NanoCanvas1;

  void setCursor(lcdint_t x, lcdint_t y) {
    m_cursorX = x;
    m_cursorY = y;
  }
};
Canvas canvas;

const uint8_t dispCross[] PROGMEM = {
    0b10001,
    0b01010,
    0b00100,
    0b01010,
    0b10001,
};

void dispCb() {
  canvas.clear();
  for (uint8_t i = 0; i < 4; i++) {
    canvas.drawRect(i * 32 + 0, 0, i * 32 + 30, 30);

    uint8_t x = i * 32 + map(speeds[i * 2 + 1], 0, 255, 1, 29);
    uint8_t y = map(speeds[i * 2], 0, 255, 29, 1);
    if (controlGetBrakeMask() & _BV(i)) {
      canvas.drawBitmap1(x - 2, y - 2, 5, 8, dispCross);
    } else {
      canvas.fillRect(x - 1, y - 1, x + 1, y + 1);
    }

    canvas.setCursor(i * 32 + 3, 32);
    if (isPresent(i + 1)) {
      canvas.print(vbats[i] / 1000.0);
    } else {
      canvas.print("----");
    }
  }

  if (boost) {
    canvas.setCursor(0, 48);
    canvas.print("BOOST");
  }

  canvas.setCursor(0, 56);
  canvas.print("C:");
  if (isPresent(255)) {
    canvas.print(vbats[4] / 1000.0);
  } else {
    canvas.print("----");
  }
  canvas.print(" R:");
  canvas.print(controlGetVbat() / 1000.0);

  canvas.blt();
}
Task dispTask(100, TASK_FOREVER, &dispCb);
Task presenceTask(1000, TASK_FOREVER, &updatePresence);

void logicSetup(Scheduler *scheduler) {
  memset(speeds, 128, sizeof(speeds));
  canvas.begin(128, 64, framebuffer);
  canvas.clear();

  scheduler->addTask(dispTask);
  scheduler->addTask(presenceTask);
  dispTask.enable();
  presenceTask.enable();
}

#endif