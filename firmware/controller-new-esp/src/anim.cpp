#include "anim.h"
#include <Ticker.h>
#include "control.h"

Ticker animTicker;
AnimState animState;


void animTickerCb() {
  switch (animState) {
    case AnimSwipeRed:
      Serial.print("Abcd");
      animState = AnimSwipeBlue;
      break;
    case AnimSwipeBlue:
      Serial.print("aBcd");
      animState = AnimSwipeYellow;
      break;
    case AnimSwipeYellow:
      Serial.print("abCd");
      animState = AnimSwipeGreen;
      break;
    case AnimSwipeGreen:
      Serial.print("abcD");
      animState = AnimSwipeRed;
      break;

    case AnimBlinkBrake:
      Serial.write(controlGetBrakeMask() & 1 ? 'a' : 'A');
      Serial.write(controlGetBrakeMask() & 2 ? 'b' : 'B');
      Serial.write(controlGetBrakeMask() & 4 ? 'c' : 'C');
      Serial.write(controlGetBrakeMask() & 8 ? 'd' : 'D');
      animState = AnimBlinkAll;
      break;
    case AnimBlinkAll:
      Serial.print("ABCD");
      animState = AnimBlinkBrake;
      break;

    default:
      Serial.print("abcd");
      animState = AnimBlinkBrake;
      break;
  }
}

void animSetup() {
  animState = AnimSwipeRed;
  animTickerCb();
  animTicker.attach(0.25, &animTickerCb);
}

void animSetState(AnimState state) {
  animState = state;
}
