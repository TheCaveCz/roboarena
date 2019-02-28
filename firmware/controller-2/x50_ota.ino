
void otaSetup() {
  String ssid = HOSTNAME_PREFIX;
  ssid += chipId;
  ArduinoOTA.setHostname(ssid.c_str());

#ifdef OTA_PASSWORD
  ArduinoOTA.setPassword(OTA_PASSWORD);
#endif
  
  ArduinoOTA.onStart([]() {
    logInfo("Starting OTA");
  });
  ArduinoOTA.begin();
}
