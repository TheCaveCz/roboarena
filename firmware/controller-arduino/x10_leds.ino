Task ledsTask(250, TASK_FOREVER, &ledsBlinkCb, &scheduler, true);

const uint8_t ledsPins[ROBOT_COUNT] = {6, 8, 10, 12};

#define LEDS_OFF 0
#define LEDS_ON 1
#define LEDS_BLINK 2
#define LEDS_SWIPE 3

uint8_t ledsState[ROBOT_COUNT];

void ledsBlinkCb() {
  for (uint8_t i = 0; i < ROBOT_COUNT; i++) {
    if (ledsState[i] == LEDS_BLINK) {
      digitalWrite(ledsPins[i], ledsTask.getRunCounter() & 1);
    } else if (ledsState[i] == LEDS_SWIPE) {
      digitalWrite(ledsPins[i], (ledsTask.getRunCounter() & 3) == i);
    }
  }
}

void ledsSetState(const uint8_t led, const uint8_t state) {
  switch (state) {
    case LEDS_BLINK:
      digitalWrite(ledsPins[led], ledsTask.getRunCounter() & 1);
      ledsState[led] = LEDS_BLINK;
      break;
    case LEDS_ON:
      digitalWrite(ledsPins[led], HIGH);
      ledsState[led] = LEDS_ON;
      break;
    case LEDS_SWIPE:
      digitalWrite(ledsPins[led], (ledsTask.getRunCounter() & 3) == led);
      ledsState[led] = LEDS_SWIPE;
      break;
    default:
      digitalWrite(ledsPins[led], LOW);
      ledsState[led] = LEDS_OFF;
      break;
  }
}

void ledsSetup() {
  for (uint8_t i = 0; i < ROBOT_COUNT; i++) {
    ledsState[i] = LEDS_OFF;
    pinMode(ledsPins[i], OUTPUT);
    digitalWrite(ledsPins[i], LOW);
  }
}
