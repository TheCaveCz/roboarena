
WiFiEventHandler wifiIpHandler;
bool wifiShouldSaveConfig;
bool wifiGotIpFlag;

void wifiResetAndRestart() {
  logInfo("WiFi reset in progress");
  WiFi.disconnect(true);
  delay(1000);
  ESP.reset();
  while (1) delay(1);
}

void wifiStartAp() {
  WiFiManager wifiManager;

  String ssid = HOSTNAME_PREFIX;
  ssid += chipId;
  if (!wifiManager.startConfigPortal(ssid.c_str())) {
    logInfo("Failed to connect or setup, rebooting...");
    delay(1000);
    ESP.restart();
    while (1) delay(1);
  }
}

void wifiSetup() {
  if (WiFi.getAutoConnect()) WiFi.setAutoConnect(false);
  if (WiFi.getMode() != WIFI_STA) WiFi.mode(WIFI_STA);

  logInfo("Connecting to WiFi...");
  Serial.print("RC\n");
  wifiGotIpFlag = false;

  wifiIpHandler = WiFi.onStationModeGotIP([](const WiFiEventStationModeGotIP & evt) {
    // this executes when module reconnects and gets IP from DHCP
    // can be called multiple times
    wifiGotIpFlag = true;
    logInfo("WiFi connected successfuly");
    logValue("Got IP: ", evt.ip);
  });

  if (WiFi.SSID() == "") {
    logInfo("No saved credentials");
    //pixelsSetAnimState(PIXELS_ANIM_VIOLET);
    wifiStartAp();
  } else if (!WiFi.isConnected()) {
    logValue("Stored SSID: ", WiFi.SSID());
    WiFi.begin();
  }
}
