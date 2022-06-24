#include "control.h"
#include "Commander.h"
#include <log.h>

Commander cmd;

uint32_t adcVbat;
uint8_t controlPos[8];
uint8_t controlBrakeMask;
ControlButtonCallback buttonCallback;

uint8_t controlPosTable[256] = {
    0, 2, 4, 6, 8, 10, 14, 16, 17, 19, 21, 23, 25, 27, 28, 30,                      //
    32, 34, 37, 39, 40, 42, 44, 45, 47, 48, 50, 52, 53, 55, 58, 59,                 //
    61, 62, 64, 65, 66, 68, 69, 70, 72, 73, 76, 77, 78, 79, 81, 82,                 //
    83, 84, 85, 87, 88, 89, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100,                //
    101, 103, 104, 105, 105, 106, 107, 108, 109, 109, 110, 111, 112, 113, 114, 114, //
    115, 116, 116, 117, 118, 118, 119, 119, 120, 121, 121, 122, 122, 122, 123, 123, //
    124, 124, 124, 125, 125, 126, 126, 126, 126, 127, 127, 127, 127, 127, 127, 128, //
    128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, //
    128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, //
    128, 129, 129, 129, 129, 129, 129, 130, 130, 130, 130, 131, 131, 132, 132, 132, //
    133, 133, 133, 134, 134, 135, 135, 136, 137, 137, 138, 138, 139, 140, 140, 141, //
    141, 142, 143, 144, 145, 146, 146, 147, 148, 149, 150, 150, 151, 152, 153, 155, //
    156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 168, 169, 170, 171, 173, //
    174, 175, 176, 177, 179, 180, 181, 184, 185, 186, 188, 189, 191, 192, 193, 195, //
    196, 198, 201, 202, 204, 205, 207, 208, 210, 212, 213, 215, 217, 218, 222, 223, //
    225, 227, 229, 230, 232, 234, 236, 238, 240, 241, 245, 247, 249, 251, 253, 255, //
};

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
    controlPos[i] = controlPosTable[cmd.parseHex(buf[1 + i * 2], buf[2 + i * 2])];
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
