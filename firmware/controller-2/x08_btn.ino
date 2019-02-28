Task btnTask(50, TASK_FOREVER, &btnTick, &scheduler, true);

uint8_t btnBrakeMask;
uint8_t btnConnectFlag;
uint8_t btnLast;
uint8_t btnCodeToSend;

void btnUpdateLed(const uint8_t i) {
  if (btnConnectFlag) {
    ledsSetState(i, LEDS_SWIPE);
  } else {
    ledsSetState(i, btnBrakeMask & (1 << i) ? LEDS_BLINK : LEDS_ON);
  }
}

void btnUpdateLeds() {
  for (uint8_t i = 0; i < ROBOT_COUNT; i++) {
    btnUpdateLed(i);
  }
}

uint8_t btnFromAnalog(const int analog) {
  switch (analog / 64) {
    case 5:
      return 4;
    case 6:
    case 7:
      return 3;
    case 8:
      return 2;
    case 9:
    case 10:
      return 1;
    case 12:
    case 13:
      return 5;
    case 15:
    case 16:
      return 6;
    default:
      return 0;
  }
}

void btnTick() {
  uint8_t btn = btnFromAnalog(analogRead(A0));
  if (btn == btnLast) return;
  btnLast = btn;
  if (btn == 0) return;

  btn--;
  switch (btn) {
    case 4:
      btnCodeToSend = 4;
      break;
    case 5:
      btnCodeToSend = 9;
      break;
    default:
      btnCodeToSend = btn + ((btnBrakeMask & (1 << btn)) ? 5 : 0);
      break;
  }

  
}

void btnSetup() {
  btnBrakeMask = 0;
  btnConnectFlag = 0;
  btnLast = 0;
  btnCodeToSend = 255;
  btnUpdateLeds();
}
