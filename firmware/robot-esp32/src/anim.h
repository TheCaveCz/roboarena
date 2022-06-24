#pragma once

#include <Arduino.h>
#include <TaskSchedulerDeclarations.h>

enum AnimMode: uint8_t {
  AnimModeConnecting,
  AnimModeNormal,
  AnimModeBrake,
  AnimModeBoost,
};

enum AnimColor: uint8_t {
  AnimColorUnknown = 0,
  AnimColorRed = 1,
  AnimColorBlue = 2,
  AnimColorYellow = 3,
  AnimColorGreen = 4,
};

void animSetup(Scheduler *scheduler);
void animTick();
void animSetMode(AnimMode mode);
void animSetHighlight(float amount);
void animSetColor(AnimColor color);

