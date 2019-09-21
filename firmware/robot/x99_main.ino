
void setup() {
  Serial.begin(115200);
  logInfo("Roboarena!!!");

  pixelsSetup();
  configSetup();
  motorsSetup();
  readerSetup();
  logicSetup();
  otaSetup();
  wifiSetup();
  controlSetup();
}

void loop() {
  ArduinoOTA.handle();
  scheduler.execute();
  logicResetButton.tick();

  if (wifiGotIpFlag) {
    wifiGotIpFlag = false;
    logicUpdateRoboId();
  }
}
