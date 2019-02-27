OneButton resetButton(D6, true);


void resetPressStart() {
  logInfo("Reset long press start");
  pixelsSetAnimState(PIXELS_ANIM_YELLOW);
}

void resetPressStop() {
  logInfo("Reset long press stop");
  wifiResetAndRestart();
}

void setup() {
  Serial.begin(115200);
  logInfo("Roboarena!!!");

  pinMode(D5, OUTPUT);
  digitalWrite(D5, LOW);

  resetButton.setPressTicks(3000);
  resetButton.attachLongPressStart(resetPressStart);
  resetButton.attachLongPressStop(resetPressStop);

  pixelsSetup();
  otaSetup();
  wifiSetup();
  controlSetup();
}

void loop() {
  ArduinoOTA.handle();
  scheduler.execute();
  resetButton.tick();

  if (wifiGotIpFlag) {
    wifiGotIpFlag = false;
    pixelsSetAnimState(PIXELS_ANIM_NORMAL);
  }
}
