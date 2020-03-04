#pragma once

#include <Arduino.h>
#include <TaskSchedulerDeclarations.h>

enum ButtonEventType : uint8_t {
  EventPress = 0,
  EventLongStart = 1,
  EventLongStop = 2,
};

typedef void (*ControlButtonCallback)(uint8_t buttonId, ButtonEventType et);

void controlSetup(Stream *serial, Scheduler *scheduler, ControlButtonCallback buttonCallback);
uint16_t controlGetVbat();
uint8_t *controlGetPos();
uint8_t controlGetBrakeMask();
void controlSetBrakeMask(uint8_t bm);
