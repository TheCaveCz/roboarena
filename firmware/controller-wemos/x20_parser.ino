#define DEADZONE 30

Task parserTask(5, TASK_FOREVER, &parserMsgStartCb, &scheduler, true);

uint8_t parserSpeedsRaw[8];
uint8_t parserSpeeds[8];
uint8_t parserBrakeMask;

void parserMsgStartCb() {
  // discard all received data until we find sequence start marker
  while (Serial.available()) {
    if (Serial.read() == 'R') {
      parserTask.setCallback(&parserMsgReadCb);
      return;
    }
  }
}

void parserDump() {
  int b;
  while (Serial.available()) {
    b = Serial.read();
    if (b < 16) Serial.write(' ');
    Serial.print(b, HEX);
    Serial.write(' ');
  }
  Serial.println();
}

void parserSetup() {
  memset(parserSpeedsRaw, 128, sizeof(parserSpeedsRaw));
  parserNormalizeSpeeds();
  parserBrakeMask = 0;
}

uint8_t parserConvertHex(uint8_t c) {
  switch (c) {
    case '0'...'9': return c - '0';
    case 'a'...'f': return c - 'a' + 10;
    case 'A'...'F': return c - 'A' + 10;
    default: return 255;
  }
}

int parserGetByte() {
  uint8_t c1 = parserConvertHex(Serial.read());
  uint8_t c2 = parserConvertHex(Serial.read());
  if (c1 == 255 || c2 == 255) return -1;

  return (c1 << 4) | c2;
}

void parserMsgReadCb() {
  if (Serial.available() < 18) {
    // not enough bytes in buffer, wait for some more
    return;
  }

  uint8_t speeds[8];
  uint8_t brakeMask;
  uint8_t resetFlag;

  for (uint8_t i = 0; i < 8; i++) {
    int b = parserGetByte();
    if (b == -1) {
      logInfo("Failed to read speeds");
      parserDump();
      parserTask.setCallback(&parserMsgStartCb);
      return;
    }
    speeds[i] = b;
  }

  brakeMask = parserConvertHex(Serial.read());
  if (brakeMask == 255) {
    logInfo("Failed to read brake mask");
    parserDump();
    parserTask.setCallback(&parserMsgStartCb);
    return;
  }

  resetFlag = parserConvertHex(Serial.read());
  if (resetFlag == 255) {
    logInfo("Failed to read resetFlag");
    parserDump();
    parserTask.setCallback(&parserMsgStartCb);
    return;
  }

  if (Serial.read() != 10) {
    logInfo("Failed to read line terminator");
    parserDump();
    parserTask.setCallback(&parserMsgStartCb);
    return;
  }

  //  logHeader();
  //  for (uint8_t i = 0; i < 8; i++) {
  //    logRaw(speeds[i]);
  //    logRaw(' ');
  //  }
  //  logRaw(brakeMask);
  //  logLine();

  memcpy(parserSpeedsRaw, speeds, sizeof(parserSpeedsRaw));
  parserNormalizeSpeeds();
  parserBrakeMask = brakeMask;


  if (resetFlag) {
    Serial.print("RX\n");
    Serial.flush();
    wifiResetAndRestart();
  }

  parserTask.setCallback(&parserMsgStartCb);
}

uint8_t parserNormalize(const uint8_t in) {
  if (in <= 128 - DEADZONE) {
    return map(in, 0, 128 - DEADZONE, 0, 127);
  } else if (in >= 128 + DEADZONE) {
    return map(in, 128 + DEADZONE, 255, 129, 255);
  } else {
    return 128;
  }
}

void parserNormalizeSpeeds() {
  for (uint8_t i = 0; i < 8; i++) {
    parserSpeeds[i] = parserNormalize(parserSpeedsRaw[i]);
  }
}
