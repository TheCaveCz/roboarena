
void setup() {
  txSetup();
  ledsSetup();
  btnSetup();
  adcSetup();
}

void loop() {
  btnTick();
  scheduler.execute();
}
