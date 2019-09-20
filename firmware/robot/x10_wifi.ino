
#define MASTER_RESET_DELAY 20
#define MASTER_RESET_COUNTER 200

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
  wifiShouldSaveConfig = false;
  wifiManager.setSaveConfigCallback([]() {
    logInfo("Config should be saved");
    wifiShouldSaveConfig = true;
  });

  String roboIdString = String(config.roboId);
  WiFiManagerParameter roboIdParam("roboid", "Robot ID", roboIdString.c_str(), 2);
  wifiManager.addParameter(&roboIdParam);

  String pwm1String = String(config.m1maxPwm);
  WiFiManagerParameter pwm1Param("m1pwm", "M1 PWM", pwm1String.c_str(), 4);
  wifiManager.addParameter(&pwm1Param);

  String pwm2String = String(config.m2maxPwm);
  WiFiManagerParameter pwm2Param("m2pwm", "M2 PWM", pwm2String.c_str(), 4);
  wifiManager.addParameter(&pwm2Param);

  String ssid = HOSTNAME_PREFIX;
  ssid += chipId;
  if (!wifiManager.startConfigPortal(ssid.c_str())) {
    logInfo("Failed to connect or setup, rebooting...");
    delay(1000);
    ESP.restart();
    while (1) delay(1);
  }

  config.roboId = atoi(roboIdParam.getValue());
  config.m1maxPwm = atoi(pwm1Param.getValue());
  config.m2maxPwm = atoi(pwm2Param.getValue());

  if (wifiShouldSaveConfig) {
    configWrite();
  }
}

void wifiSetup() {
  if (WiFi.getAutoConnect()) WiFi.setAutoConnect(false);
  if (WiFi.getMode() != WIFI_STA) WiFi.mode(WIFI_STA);

  logInfo("Connecting to WiFi...");
  pixelsSetAnimState(PIXELS_ANIM_BLUE, 0);
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
    pixelsSetAnimState(PIXELS_ANIM_VIOLET, 0);
    wifiStartAp();
  } else if (!WiFi.isConnected()) {
    logValue("Stored SSID: ", WiFi.SSID());
    WiFi.begin();
  }
}
