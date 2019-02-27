uint8_t logicBrake;
OneButton logicResetButton(D8, false, false);

void logicSetSpeed(uint8_t sx, uint8_t sy) {
  if (logicBrake) return;

  int p = sx;
  int q = sy - 128;
  int m1 = constrain(p + q, 0, 255);
  int m2 = constrain(p - q, 0, 255);

  if (m1targetSpeed != m1 || m2targetSpeed != m2) {
    logValue("Speed m1: ", m1);
    logValue("Speed m2: ", m2);
  }

  m1targetSpeed = m1;
  m2targetSpeed = m2;
}

void logicSetBrake(uint8_t brake) {
  brake = brake != 0;
  if (logicBrake == brake) return;

  logicBrake = brake;
  if (logicBrake) {
    m1targetSpeed = 128;
    m2targetSpeed = 128;
  }
  logicUpdateAnimState();

  logValue("Brake: ", logicBrake);
}

void logicUpdateAnimState() {
  if (logicResetButton.isLongPressed()) {
    pixelsSetAnimState(PIXELS_ANIM_YELLOW);
  } else {
    pixelsSetAnimState(logicBrake ? PIXELS_ANIM_BLINK : PIXELS_ANIM_NORMAL);
  }
}

void logicUpdateRoboId() {
  if (configIdValid()) {
    uint32_t c = configColorForId();
    pixelsSetColor(c >> 16, (c >> 8) & 0xff, c & 0xff);
    logicUpdateAnimState();
  } else {
    pixelsSetAnimState(PIXELS_ANIM_RED);
  }
}

void logicResetPressStart() {
  logInfo("Reset long press start");
  logicUpdateAnimState();
}

void logicResetPressStop() {
  logInfo("Reset long press stop");
  wifiResetAndRestart();
}

void logicSetup() {
  logicBrake = 0;

  logicResetButton.setPressTicks(3000);
  logicResetButton.attachLongPressStart(logicResetPressStart);
  logicResetButton.attachLongPressStop(logicResetPressStop);
}
