#include "anim.h"
#include <Ticker.h>
#include "control.h"

Ticker animTicker;
AnimState animState;
uint8_t animPresence;

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
      Serial.write(controlGetBrakeMask() & 1 ? 'a' : (animPresence & 1 ? 'A' : 'a'));
      Serial.write(controlGetBrakeMask() & 2 ? 'b' : (animPresence & 2 ? 'B' : 'b'));
      Serial.write(controlGetBrakeMask() & 4 ? 'c' : (animPresence & 4 ? 'C' : 'c'));
      Serial.write(controlGetBrakeMask() & 8 ? 'd' : (animPresence & 8 ? 'D' : 'd'));
      animState = AnimBlinkAll;
      break;

    case AnimBlinkAll:
      Serial.write(animPresence & 1 ? 'A' : 'a');
      Serial.write(animPresence & 2 ? 'B' : 'b');
      Serial.write(animPresence & 4 ? 'C' : 'c');
      Serial.write(animPresence & 8 ? 'D' : 'd');
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

void animSetState(AnimState state) { animState = state; }

void animSetPresence(uint8_t p) { animPresence = p; }
