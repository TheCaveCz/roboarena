OneButton logicResetButton(D8, false, false);

uint8_t logicBrake;
uint8_t logicLife;

Task logicChipTimeoutTask(500, 1, &logicChipTimeoutCb, &scheduler, false);
Task logicInvulnTask(3000, 1, &logicInvulnCb, &scheduler, false);

uint32_t logicLastChip;
uint8_t logicChipReadCount;
uint8_t logicInvuln;


void logicChipTimeoutCb() {
  //logInfo("Chip timeout cb");
  logicLastChip = 0;
  logicChipReadCount = 0;
}

void readerChipEvent(const uint32_t chip) {
  if (logicLastChip != chip) {
    logicLastChip = chip;
    logicChipReadCount = 0;
  }
  logicChipReadCount++;
  //logValue("Chip read count: ", logicChipReadCount);
  if (logicChipReadCount < 3) {
    logicChipTimeoutTask.restartDelayed(0);
    return;
  }

  logicChipReadCount = 0;
  logicChipTimeoutTask.disable();
  uint8_t enemyId = configIdForChip(chip);
  if (enemyId) {
    logicTakeHit(enemyId);
  }
}

void logicTakeHit(const uint8_t enemyId) {
  if (logicInvuln || logicLife == 0 || logicBrake) return;

  logValue("Hit by enemy id:", enemyId);

  if (logicLife > 1) {
    logicLife--;
    logicStartInvulnerability();
  } else {
    logicLife = 0;
    m1targetSpeed = 128;
    m2targetSpeed = 128;
    logicUpdateAnimState();
    controlSendBrakeRequest();
  }
  controlSendHit(enemyId, logicLife);
}

void logicStartInvulnerability() {
  logInfo("Start invuln");
  logicInvuln = true;
  logicInvulnTask.restartDelayed(0);
  logicUpdateAnimState();
}

void logicInvulnCb() {
  logInfo("End invuln");
  logicInvuln = false;
  logicUpdateAnimState();
}

void logicSetLife(const uint8_t life) {
  if (life == 0 || life > 3) return;

  logValue("Life set to: ", life);
  logicLife = life;
  logicUpdateAnimState();
}

void logicSetSpeed(uint8_t sx, uint8_t sy) {
  if (logicBrake || logicLife == 0) return;

  int p = sx;
  int q = sy - 128;
  int m1 = constrain(p + q, 0, 255);
  int m2 = constrain(p - q, 0, 255);

  if (m1targetSpeed != m1 || m2targetSpeed != m2) {
    //    logValue("Speed m1: ", m1);
    //    logValue("Speed m2: ", m2);
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
    pixelsSetAnimState(PIXELS_ANIM_YELLOW, 0);
  } else if (logicBrake) {
    pixelsSetAnimState(logicLife ? PIXELS_ANIM_BLINK : PIXELS_ANIM_BLINK_SPLIT, logicLife);
  } else if (logicInvuln) {
    pixelsSetAnimState(PIXELS_ANIM_BLINK_FAST, logicLife);
  } else {
    pixelsSetAnimState(PIXELS_ANIM_NORMAL, logicLife);
  }
}

void logicUpdateRoboId() {
  if (configIdValid()) {
    uint32_t c = configColorForId();
    pixelsSetColor(c >> 16, (c >> 8) & 0xff, c & 0xff);
    logicUpdateAnimState();
  } else {
    pixelsSetAnimState(PIXELS_ANIM_RED, 0);
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
  logicLife = 3;
  logicInvuln = false;

  logicResetButton.setPressTicks(3000);
  logicResetButton.attachLongPressStart(logicResetPressStart);
  logicResetButton.attachLongPressStop(logicResetPressStop);
}
