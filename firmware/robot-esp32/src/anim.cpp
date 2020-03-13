#include "anim.h"
#include "pinmap.h"
#include <NeoPixelBus.h>

NeoPixelBus<NeoGrbFeature, NeoWs2812xMethod> strip(16, PIN_LEDS_WS);

AnimMode mode;
AnimColor color;
float highlight;
float scale;

#define BASE_VALUE 128
void animCb();
Task animTask(20, TASK_FOREVER, &animCb);
NeoGamma<NeoGammaTableMethod> neoGamma;

RgbColor animGetRgb(float scale) {
  float base = ((float)BASE_VALUE) * (1.0f + highlight) * scale;
  if (base > 255)
    base = 255;

  switch (color) {
    case AnimColorRed:
      return neoGamma.Correct(RgbColor(base, 0, 0));
    case AnimColorGreen:
      return neoGamma.Correct(RgbColor(0, base, 0));
    case AnimColorBlue:
      return neoGamma.Correct(RgbColor(0, 0, base));
    case AnimColorYellow:
      return neoGamma.Correct(RgbColor(base, base * 0.7f, 0));
    default:
      return neoGamma.Correct(RgbColor(base, 0, base));
  }
}

void animRefreshColors() {
  if (mode != AnimModeConnecting) {
    strip.ClearTo(animGetRgb(scale));
  }
}

void animCb() {
  if (mode == AnimModeConnecting) {
    strip.RotateLeft(1);
    return;
  }

  if (mode == AnimModeBrake) {
    scale = animTask.getRunCounter() % 40;
    scale = NeoEase::QuadraticInOut(abs((scale / 39.0f) * 2.0f - 1.0f));
  }
  animRefreshColors();
}

void animTick() {
  if (strip.CanShow())
    strip.Show();
}

void animSetup(Scheduler *scheduler) {
  color = AnimColorUnknown;
  highlight = 0;

  strip.Begin();
  strip.Show();

  scheduler->addTask(animTask);
  animSetMode(AnimModeNormal);
}

void animSetMode(AnimMode m) {
  switch (m) {
    case AnimModeNormal:
      mode = m;
      scale = 1;
      animRefreshColors();
      animTask.disable();
      break;

    case AnimModeConnecting:
      mode = m;
      scale = 1;
      strip.ClearTo(RgbColor(0, 0, 0));
      for (uint8_t i = 0; i <= 5; i++) {
        strip.SetPixelColor(i, animGetRgb(1 - ((float)i) / 5.0f));
      }
      animTask.setInterval(80);
      animTask.restart();
      break;

    case AnimModeBrake:
      mode = m;
      animTask.setInterval(20);
      animTask.restart();
      break;

    default:
      return;
  }
}

void animSetHighlight(float h) {
  highlight = constrain(h, 0.0f, 1.0f);
  animRefreshColors();
}

void animSetColor(AnimColor c) {
  color = c;
  animRefreshColors();
}
