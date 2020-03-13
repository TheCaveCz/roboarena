#include "control.h"
#include "Commander.h"
#include <log.h>

Commander cmd;

uint32_t adcVbat;
uint8_t controlPos[8];
uint8_t controlBrakeMask;
ControlButtonCallback buttonCallback;

void adcCb() {
  adcVbat = analogRead(A0) *
#if LOGIC_REMOTE
            4230
#elif LOGIC_CONTROLLER
            4150
#else
            4300
#endif
            / 1024;
}
Task adcTask(1000, TASK_FOREVER, &adcCb);

void cmdCb() { cmd.check(); }
Task cmdTask(5, TASK_FOREVER, &cmdCb);


COMMAND(cmdButton) {
  uint8_t id = cmd.parseHex(buf[1]);
  if (!cmd.checkRange(id, 1, 6)) {
    logValue("invalid button id = ", id);
    return;
  }

  if (buttonCallback) {
    buttonCallback(id, buf[0] == 'X' ? EventPress : (buf[0] == 'Y' ? EventLongStart : EventLongStop));
  }
}

COMMAND(cmdControls) {
  for (uint8_t i = 0; i < 8; i++) {
    controlPos[i] = cmd.parseHex(buf[1 + i * 2], buf[2 + i * 2]);
    logValue2("ctrl ", i, "val=", controlPos[i]);
  }
}

const Command commands[] = {
    {'X', 2, cmdButton},    //
    {'Y', 2, cmdButton},    //
    {'y', 2, cmdButton},    //
    {'Z', 17, cmdControls}, //
    {0, 0, NULL}            //
};

uint16_t controlGetVbat() { return adcVbat; }
uint8_t *controlGetPos() { return controlPos; }
uint8_t controlGetBrakeMask() { return controlBrakeMask; }
void controlSetBrakeMask(uint8_t bm) { controlBrakeMask = bm & 0xf; }

void controlSetup(Stream *serial, Scheduler *scheduler, ControlButtonCallback bcb) {
  buttonCallback = bcb;
  controlBrakeMask = 0; // 0b1110;
  memset(controlPos, 128, sizeof(controlPos));
  cmd.begin(commands, serial);

  scheduler->addTask(adcTask);
  adcTask.enable();
  scheduler->addTask(cmdTask);
  cmdTask.enable();
}
