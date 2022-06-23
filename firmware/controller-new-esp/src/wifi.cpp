#include "wifi.h"
#if USE_WIFI

#include <ESP8266WiFi.h>
#include <WiFiManager.h>
#include <Ticker.h>
#include <log.h>

WiFiEventHandler wifiIpHandler;
bool wifiGotIpFlag;

void wifiResetAndRestart() {
  logInfo("WiFi reset in progress");
  WiFi.disconnect(true);
  delay(1000);
  ESP.reset();
  while (1)
    delay(1);
}

bool wifiHasIp() {
  bool r = wifiGotIpFlag;
  wifiGotIpFlag = false;
  return r;
}

void wifiStartAp(const String &ssid) {
  WiFiManager wifiManager;

  if (!wifiManager.startConfigPortal(ssid.c_str())) {
    logInfo("Failed to connect or setup, rebooting...");
    delay(1000);
    ESP.restart();
    while (1)
      delay(1);
  }
}

void wifiSetup(const String &ssid) {
  if (WiFi.getAutoConnect())
    WiFi.setAutoConnect(false);
  if (WiFi.getMode() != WIFI_STA)
    WiFi.mode(WIFI_STA);

  logInfo("Connecting to WiFi...");
  wifiGotIpFlag = false;

  wifiIpHandler = WiFi.onStationModeGotIP([](const WiFiEventStationModeGotIP &evt) {
    // this executes when module reconnects and gets IP from DHCP
    // can be called multiple times
    wifiGotIpFlag = true;
    logInfo("WiFi connected successfuly");
    logValue("Got IP: ", evt.ip);
  });

  if (WiFi.SSID() == "") {
    logInfo("No saved credentials");
    wifiStartAp(ssid);
  } else if (!WiFi.isConnected()) {
    logValue("Stored SSID: ", WiFi.SSID());
    WiFi.begin();
  }
}
#endif