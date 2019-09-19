uint8_t otaProgress;

void otaSetup() {
  String ssid = HOSTNAME_PREFIX;
  ssid += chipId;
  ArduinoOTA.setHostname(ssid.c_str());

#ifdef OTA_PASSWORD
  ArduinoOTA.setPassword(OTA_PASSWORD);
#endif

  ArduinoOTA.onStart([]() {
    logInfo("OTA starting");
    logicSetBrake(1);
    pixelsSetAnimState(PIXELS_ANIM_GREEN, 3);
  });
  ArduinoOTA.onEnd([]() {
    logInfo("OTA finished");
  });
  ArduinoOTA.onProgress([](uint32_t now, uint32_t total) {
    uint8_t p = map(now, 0, total, 0, 10);
    if (p != otaProgress) {
      logValue("OTA: ", p * 10);
      otaProgress = p;
    }
  });
  ArduinoOTA.begin();
}
