
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

uint8_t pixelsBuffer[PIXELS_BYTE_COUNT];
uint8_t pixelsTargetR;
uint8_t pixelsTargetG;
uint8_t pixelsTargetB;

Ticker pixelsTicker;

uint16_t pixelsProgress;
uint8_t pixelsState;


void pixelsSend(uint8_t r, uint8_t g, uint8_t b) {
  for (int i = 0; i < PIXELS_BYTE_COUNT; i += 3) {
    pixelsBuffer[i] = r;
    pixelsBuffer[i + 1] = g;
    pixelsBuffer[i + 2] = b;
  }

  ws2812Send(pixelsBuffer, PIXELS_BYTE_COUNT);
}

void pixelsTick() {
  if (pixelsState == PIXELS_ANIM_NORMAL) {
    pixelsSend(pixelsTargetR, pixelsTargetG, pixelsTargetB);
    return;
  }

  uint8_t v = pixelsProgress < (PIXELS_ANIM_MAX_RANGE / 2)
              ? map(pixelsProgress, 0, (PIXELS_ANIM_MAX_RANGE / 2) - 1, 4, 128)
              : map(pixelsProgress, PIXELS_ANIM_MAX_RANGE / 2, PIXELS_ANIM_MAX_RANGE - 1, 128, 4);

  switch (pixelsState) {
    case PIXELS_ANIM_BLUE:
      pixelsSend(0, 0, v);
      break;
    case PIXELS_ANIM_VIOLET:
      pixelsSend(v, 0, v);
      break;
    case PIXELS_ANIM_GREEN:
      pixelsSend(0, v, 0);
      break;
    case PIXELS_ANIM_YELLOW:
      pixelsSend(v, v, 0);
      break;
    case PIXELS_ANIM_BLINK:
      if ((pixelsProgress % 20) < 10) {
        pixelsSend(pixelsTargetR, pixelsTargetG, pixelsTargetB);
      } else {
        pixelsSend(0, 0, 0);
      }
      break;
    default:
      pixelsSend(v, 0, 0);
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

void pixelsSetAnimState(uint8_t v) {
  logValue("Pixels anim state set to ", v);
  pixelsState = v;
  pixelsProgress = 0;
}

void pixelsSetup() {
  ws2812Setup();

  pixelsSetColor(0, 0, 0);

  pixelsProgress = 0;
  pixelsState = PIXELS_ANIM_BLUE;
  pixelsTicker.attach_ms(PIXELS_TASK_INTERVAL, pixelsTick);
}
