
void setup() {
  Serial.begin(115200);
  logInfo("Roboarena ctrl2!!!");

  ledsSetup();
  btnSetup();
  otaSetup();
  wifiSetup();
  controlSetup();
}

void loop() {
  ArduinoOTA.handle();
  scheduler.execute();
}
