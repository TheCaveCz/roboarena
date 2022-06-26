#include <Arduino.h>
#include <TaskScheduler.h>
#include <Ticker.h>
#include <log.h>
#include <sender.h>
#include <NeoPixelBus.h>
#include <NeoPixelAnimator.h>

#define PIX_CNT 50

Scheduler scheduler;
NeoPixelBus<NeoRgbFeature, NeoEsp8266Uart1Ws2811Method> strip(PIX_CNT, 0);
NeoGamma<NeoGammaTableMethod> colorGamma;
NeoPixelAnimator animator(4, NEO_MILLISECONDS);

RgbColor colors[4] = {RgbColor(255, 0, 0), RgbColor(0, 0, 255), RgbColor(255, 178, 0), RgbColor(0, 255, 0)};

uint8_t brakeMask = 0;

void senderReceive(ProtocolCmd cmd, const void *buffer, size_t len) {
  if (cmd == ProtocolCmdMove) {
    const ProtocolMsgMove *msg = (const ProtocolMsgMove *)buffer;
    brakeMask = msg->brakeMask;
  }
}

float ringProgressNormalize(float progress) {
  while (progress > 1.0) {
    progress -= 1.0;
  }
  return progress;
}

RgbColor ringBlend4(const RgbColor c1, const RgbColor c2, const RgbColor c3, const RgbColor c4, float progress) {
  if (progress < 1.0 / 4.0) {
    return RgbColor::LinearBlend(c1, c2, NeoEase::ExponentialInOut(progress * 4.0));
  } else if (progress >= 3.0 / 4.0) {
    return RgbColor::LinearBlend(c4, c1, NeoEase::ExponentialInOut(progress * 4.0 - 3.0));
  } else if (progress >= 2.0 / 4.0) {
    return RgbColor::LinearBlend(c3, c4, NeoEase::ExponentialInOut(progress * 4.0 - 2.0));
  } else {
    return RgbColor::LinearBlend(c2, c3, NeoEase::ExponentialInOut(progress * 4.0 - 1.0));
  }
}

RgbColor ringBlend3(const RgbColor &c1, const RgbColor &c2, const RgbColor &c3, float progress) {
  if (progress < 1.0 / 3.0) {
    return RgbColor::LinearBlend(c1, c2, NeoEase::ExponentialInOut(progress * 3.0));
  } else if (progress >= 2.0 / 3.0) {
    return RgbColor::LinearBlend(c3, c1, NeoEase::ExponentialInOut(progress * 3.0 - 2.0));
  } else {
    return RgbColor::LinearBlend(c2, c3, NeoEase::ExponentialInOut(progress * 3.0 - 1.0));
  }
}

RgbColor ringBlend2(const RgbColor &c1, const RgbColor &c2, float progress) {
  if (progress < 1.0 / 2.0) {
    return RgbColor::LinearBlend(c1, c2, NeoEase::ExponentialInOut(progress * 2.0));
  } else {
    return RgbColor::LinearBlend(c2, c1, NeoEase::ExponentialInOut(progress * 2.0 - 1.0));
  }
}

RgbColor ringBlend(float progress) {
  switch (brakeMask & 0xf) {
    case 0b0000: // 0
      return ringBlend4(colors[0], colors[1], colors[2], colors[3], progress);
    case 0b0001: // 1
      return ringBlend3(colors[1], colors[2], colors[3], progress);
    case 0b0010: // 2
      return ringBlend3(colors[0], colors[2], colors[3], progress);
    case 0b0011: // 3
      return ringBlend2(colors[2], colors[3], progress);
    case 0b0100: // 4
      return ringBlend3(colors[0], colors[1], colors[3], progress);
    case 0b0101: // 5
      return ringBlend2(colors[1], colors[3], progress);
    case 0b0110: // 6
      return ringBlend2(colors[0], colors[3], progress);
    case 0b0111: // 7
      return ringBlend2(colors[3], 0, progress);
    case 0b1000: // 8
      return ringBlend3(colors[0], colors[1], colors[2], progress);
    case 0b1001: // 9
      return ringBlend2(colors[1], colors[2], progress);
    case 0b1010: // a
      return ringBlend2(colors[0], colors[2], progress);
    case 0b1011: // b
      return ringBlend2(colors[2], 0, progress);
    case 0b1100: // c
      return ringBlend2(colors[0], colors[1], progress);
    case 0b1101: // d
      return ringBlend2(colors[1], 0, progress);
    case 0b1110: // e
      return ringBlend2(colors[0], 0, progress);
    case 0b1111: // f
      return ringBlend4(RgbColor(255, 0, 255), 0, RgbColor(255, 0, 255), 0, progress);
  }
}

void setup() {
  Serial.begin(115200);
  logInfo("Roboarena signal");


  strip.Begin();
  animator.StartAnimation(0, 5000, [](const AnimationParam &param) {
    float rp;
    for (uint8_t i = 0; i < PIX_CNT; i++) {
      rp = ringProgressNormalize(param.progress + ((float)i / (PIX_CNT - 1)));
      strip.SetPixelColor(i, colorGamma.Correct(ringBlend(rp)));
    }
    // logValue("progress",param.progress);
    if (param.state == AnimationState_Completed) {
      animator.RestartAnimation(param.index);
    }
  });

  senderSetup(&scheduler, senderReceive, NULL);

  logInfo("setup done");
}

void loop() {
  senderTick();
  scheduler.execute();
  animator.UpdateAnimations();
  strip.Show();
}
