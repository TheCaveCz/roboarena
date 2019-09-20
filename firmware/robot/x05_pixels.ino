
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
#define PIXELS_ANIM_BLINK_SPLIT 8

uint8_t pixelsBuffer[PIXELS_BYTE_COUNT];
uint8_t pixelsTargetR;
uint8_t pixelsTargetG;
uint8_t pixelsTargetB;

Ticker pixelsTicker;

uint16_t pixelsProgress;
uint8_t pixelsLife;
uint8_t pixelsState;


void pixelsSend(uint8_t r, uint8_t g, uint8_t b, uint8_t cnt) {
  if (cnt == PIXELS_COUNT) {
    for (int i = 0; i < PIXELS_BYTE_COUNT; i += 3) {
      pixelsBuffer[i] = g;
      pixelsBuffer[i + 1] = r;
      pixelsBuffer[i + 2] = b;
    }
  } else if (cnt == 255) {
    memset(pixelsBuffer, 0, sizeof(pixelsBuffer));
    for (int i = 0; i < PIXELS_BYTE_COUNT / 2; i += 3) {
      pixelsBuffer[i] = g;
      pixelsBuffer[i + 1] = r;
      pixelsBuffer[i + 2] = b;
    }
  } else if (cnt == 254) {
    memset(pixelsBuffer, 0, sizeof(pixelsBuffer));
    for (int i = PIXELS_BYTE_COUNT / 2; i < PIXELS_BYTE_COUNT; i += 3) {
      pixelsBuffer[i] = g;
      pixelsBuffer[i + 1] = r;
      pixelsBuffer[i + 2] = b;
    }
  } else {
    memset(pixelsBuffer, 0, sizeof(pixelsBuffer));

    ///
    // 012 345 678 9ab cde f,x10,x11
    //  0   1   2   3   4   5
    //  3   3   3   3   3   3
    //      2   2   2   2
    //          1   1
    if (cnt >= 3) {
      pixelsBuffer[0] = g;
      pixelsBuffer[1] = r;
      pixelsBuffer[2] = b;

      pixelsBuffer[15] = g;
      pixelsBuffer[16] = r;
      pixelsBuffer[17] = b;
    }

    if (cnt >= 2) {
      pixelsBuffer[3] = g;
      pixelsBuffer[4] = r;
      pixelsBuffer[5] = b;

      pixelsBuffer[12] = g;
      pixelsBuffer[13] = r;
      pixelsBuffer[14] = b;
    }

    if (cnt >= 1) {
      pixelsBuffer[6] = g;
      pixelsBuffer[7] = r;
      pixelsBuffer[8] = b;

      pixelsBuffer[9] = g;
      pixelsBuffer[10] = r;
      pixelsBuffer[11] = b;
    }
  }
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
      pixelsSend(0, 0, v, PIXELS_COUNT);
      break;
    case PIXELS_ANIM_VIOLET:
      pixelsSend(v, 0, v, PIXELS_COUNT);
      break;
    case PIXELS_ANIM_GREEN:
      pixelsSend(0, v, 0, PIXELS_COUNT);
      break;
    case PIXELS_ANIM_YELLOW:
      pixelsSend(v, v, 0, PIXELS_COUNT);
      break;
    case PIXELS_ANIM_BLINK:
      if ((pixelsProgress % 20) < 10) {
        pixelsSend(pixelsTargetR, pixelsTargetG, pixelsTargetB, pixelsLife);
      } else {
        pixelsSend(0, 0, 0, 0);
      }
      break;
    case PIXELS_ANIM_BLINK_SPLIT:
      pixelsSend(pixelsTargetR, pixelsTargetG, pixelsTargetB, (pixelsProgress % 20) < 10 ? 255 : 254);
      break;

    case PIXELS_ANIM_BLINK_FAST:
      if ((pixelsProgress % 4) < 2) {
        pixelsSend(pixelsTargetR, pixelsTargetG, pixelsTargetB, pixelsLife);
      } else {
        pixelsSend(0, 0, 0, 0);
      }
      break;
    default:
      pixelsSend(v, 0, 0, PIXELS_COUNT);
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
