OneButton btnBrake0(5, true);
OneButton btnBrake1(7, true);
OneButton btnBrake2(9, true);
OneButton btnBrake3(11, true);
OneButton btnStartAll(3, true);
OneButton btnStopAll(2, true);

uint8_t btnBrakeMask;
uint8_t btnResetFlag;
uint8_t btnConnectFlag;

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

void btnBrakeToggle(const uint8_t btn) {
  if (btnBrakeMask & (1 << btn))
    btnBrakeMask &= ~(1 << btn);
  else
    btnBrakeMask |= 1 << btn;

  btnUpdateLed(btn);
}

void btnBrake0Click() {
  btnBrakeToggle(0);
}

void btnBrake1Click() {
  btnBrakeToggle(1);
}

void btnBrake2Click() {
  btnBrakeToggle(2);
}

void btnBrake3Click() {
  btnBrakeToggle(3);
}

void btnStartAllClick() {
  btnBrakeMask = 0;
  btnUpdateLeds();

}
void btnStopAllClick() {
  btnBrakeMask = 0x0f;
  btnUpdateLeds();
}

void btnStopAllLong() {
  for (uint8_t i = 0; i < ROBOT_COUNT; i++) {
    ledsSetState(i, LEDS_OFF);
  }
  adcCalibrate();
  delay(500);
  btnUpdateLeds();
}

void btnStartAllLong() {
  for (uint8_t i = 0; i < ROBOT_COUNT; i++) {
    ledsSetState(i, LEDS_OFF);
  }
  delay(200);
  for (uint8_t i = 0; i < ROBOT_COUNT; i++) {
    ledsSetState(i, LEDS_ON);
  }
  delay(200);
  for (uint8_t i = 0; i < ROBOT_COUNT; i++) {
    ledsSetState(i, LEDS_OFF);
  }
  btnResetFlag = 1;
  delay(500);
  btnUpdateLeds();
}

void btnTick() {
  btnBrake0.tick();
  btnBrake1.tick();
  btnBrake2.tick();
  btnBrake3.tick();
  btnStartAll.tick();
  btnStopAll.tick();
}

void btnSetup() {
  btnBrakeMask = 0;
  btnConnectFlag = 0;
  btnResetFlag = 0;
  btnUpdateLeds();

  btnBrake0.attachClick(btnBrake0Click);
  btnBrake1.attachClick(btnBrake1Click);
  btnBrake2.attachClick(btnBrake2Click);
  btnBrake3.attachClick(btnBrake3Click);
  btnStartAll.attachClick(btnStartAllClick);
  btnStopAll.attachClick(btnStopAllClick);

  btnStopAll.setPressTicks(2000);
  btnStopAll.attachLongPressStart(btnStopAllLong);
  btnStartAll.setPressTicks(2000);
  btnStartAll.attachLongPressStart(btnStartAllLong);
}
