#pragma once

#include <Arduino.h>
#include <TaskSchedulerDeclarations.h>

typedef void(*ControlButtonCallback)(uint8_t buttonId);

void controlSetup(Stream *serial, Scheduler *scheduler, ControlButtonCallback buttonCallback);
uint16_t controlGetVbat();
uint8_t *controlGetPos();
uint8_t controlGetBrakeMask();
void controlSetBrakeMask(uint8_t bm);
