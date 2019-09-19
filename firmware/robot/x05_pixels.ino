
#define PIXELS_BYTE_COUNT (PIXELS_COUNT * 3)
#define PIXELS_TASK_INTERVAL 25UL
#define PIXELS_ANIM_MAX_RANGE 80

#define PIXELS_ANIM_BLUE 0
#define PIXELS_ANIM_VIOLET 1
#define PIXELS_ANIM_RED 2
#define PIXELS_ANIM_GREEN 3
#define PIXELS_ANIM_YELLOW 6
#define PIXELS_ANIM_NORMAL 4
#define PIXELS_ANIM_BLINK 5
#define PIXELS_ANIM_BLINK_FAST 7

uint8_t pixelsBuffer[PIXELS_BYTE_COUNT];
uint8_t pixelsTargetR;
uint8_t pixelsTargetG;
uint8_t pixelsTargetB;

Ticker pixelsTicker;

uint16_t pixelsProgress;
uint8_t pixelsLife;
uint8_t pixelsState;


void pixelsSend(uint8_t r, uint8_t g, uint8_t b, uint8_t cnt) {
  //  for (int i = 0; i < PIXELS_BYTE_COUNT; i += 3) {
  //    pixelsBuffer[i] = g;
  //    pixelsBuffer[i + 1] = r;
  //    pixelsBuffer[i + 2] = b;
  //  }

  // zap pri cnt = 2,3
  pixelsBuffer[0] = cnt > 1 ? g : 0;
  pixelsBuffer[1] = cnt > 1 ? r : 0;
  pixelsBuffer[2] = cnt > 1 ? b : 0;

  // zap pri cnt = 1,3
  pixelsBuffer[3] = (cnt & 1) ? g : 0;
  pixelsBuffer[4] = (cnt & 1)  ? r : 0;
  pixelsBuffer[5] = (cnt & 1)  ? b : 0;

  // zap pri cnt = 2,3
  pixelsBuffer[6] = cnt > 1 ? g : 0;
  pixelsBuffer[7] = cnt > 1 ? r : 0;
  pixelsBuffer[8] = cnt > 1 ? b : 0;

  ws2812Send(pixelsBuffer, PIXELS_BYTE_COUNT);
}

void pixelsTick() {
  if (pixelsState == PIXELS_ANIM_NORMAL) {
    pixelsSend(pixelsTargetR, pixelsTargetG, pixelsTargetB, pixelsLife);
    return;
  }

  uint8_t v = pixelsProgress < (PIXELS_ANIM_MAX_RANGE / 2)
              ? map(pixelsProgress, 0, (PIXELS_ANIM_MAX_RANGE / 2) - 1, 4, 128)
              : map(pixelsProgress, PIXELS_ANIM_MAX_RANGE / 2, PIXELS_ANIM_MAX_RANGE - 1, 128, 4);

  switch (pixelsState) {
    case PIXELS_ANIM_BLUE:
      pixelsSend(0, 0, v, 3);
      break;
    case PIXELS_ANIM_VIOLET:
      pixelsSend(v, 0, v, 3);
      break;
    case PIXELS_ANIM_GREEN:
      pixelsSend(0, v, 0, 3);
      break;
    case PIXELS_ANIM_YELLOW:
      pixelsSend(v, v, 0, 3);
      break;
    case PIXELS_ANIM_BLINK:
      if ((pixelsProgress % 20) < 10) {
        pixelsSend(pixelsTargetR, pixelsTargetG, pixelsTargetB, pixelsLife);
      } else {
        pixelsSend(0, 0, 0, 0);
      }
      break;
    case PIXELS_ANIM_BLINK_FAST:
      if ((pixelsProgress % 4) < 2) {
        pixelsSend(pixelsTargetR, pixelsTargetG, pixelsTargetB, pixelsLife);
      } else {
        pixelsSend(0, 0, 0, 0);
      }
      break;
    default:
      pixelsSend(v, 0, 0, 3);
      break;
  }

  pixelsProgress++;
  if (pixelsProgress >= PIXELS_ANIM_MAX_RANGE) pixelsProgress = 0;
}

void pixelsSetColor(uint8_t r, uint8_t g, uint8_t b) {
  pixelsTargetR = r;
  pixelsTargetG = g;
  pixelsTargetB = b;
}

void pixelsSetAnimState(uint8_t v, uint8_t life) {
  logValue("Pixels anim state set to ", v);
  pixelsState = v;
  pixelsLife = life;
  pixelsProgress = 0;
}

void pixelsSetup() {
  ws2812Setup();

  pixelsSetColor(0, 0, 0);

  pixelsProgress = 0;
  pixelsState = PIXELS_ANIM_BLUE;
  pixelsTicker.attach_ms(PIXELS_TASK_INTERVAL, pixelsTick);
}
