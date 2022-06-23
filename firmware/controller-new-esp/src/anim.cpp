#include "anim.h"
#include <Ticker.h>
#include "control.h"

Ticker animTicker;
AnimState animState;
uint8_t animPresence;

void animTickerCb() {
  switch (animState) {
    case AnimSwipeRed:
      Serial.println("L1");
      animState = AnimSwipeBlue;
      break;
    case AnimSwipeBlue:
      Serial.println("L2");
      animState = AnimSwipeYellow;
      break;
    case AnimSwipeYellow:
      Serial.println("L4");
      animState = AnimSwipeGreen;
      break;
    case AnimSwipeGreen:
      Serial.println("L8");
      animState = AnimSwipeRed;
      break;

    case AnimBlinkBrake:
      Serial.write('L');
      Serial.println((~controlGetBrakeMask()) & animPresence & 0xf, HEX);
      animState = AnimBlinkAll;
      break;

    case AnimBlinkAll:
      Serial.write('L');
      Serial.println(animPresence & 0xf, HEX);
      animState = AnimBlinkBrake;
      break;

    default:
      Serial.println("L0");
      animState = AnimBlinkBrake;
      break;
  }
}

void animSetup() {
  animState = AnimSwipeRed;
  animTickerCb();
  animTicker.attach(0.25, &animTickerCb);
}

void animSetState(AnimState state) { animState = state; }

void animSetPresence(uint8_t p) { animPresence = p; }
