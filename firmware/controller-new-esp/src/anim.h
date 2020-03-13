#pragma once

#include <Arduino.h>

enum AnimState:uint8_t {
  AnimSwipeRed,
  AnimSwipeBlue,
  AnimSwipeYellow,
  AnimSwipeGreen,
  AnimBlinkBrake,
  AnimBlinkAll,
};

void animSetup();
void animSetState(AnimState state);
void animSetPresence(uint8_t p);
