
void setup() {
  Serial.begin(115200);
  logInfo("Roboarena ctrl!!!");

  otaSetup();
  parserSetup();
  wifiSetup();
  controlSetup();
}

void loop() {
  ArduinoOTA.handle();
  scheduler.execute();

  if (wifiGotIpFlag) {
    wifiGotIpFlag = false;
    Serial.print("RD\n");
  }
}
