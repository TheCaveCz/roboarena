#include "xwifi.h"

#include <WiFi.h>
#include <log.h>

/*
#pragma once

#define WIFI_SSID "ssid"
#define WIFI_PASS "pass"
*/
#include "wifi-password.h"

bool wifiGotIpFlag;

void wifiResetAndRestart() {
  logInfo("WiFi reset in progress");
  WiFi.disconnect(true);
  delay(1000);
  // ESP.reset();
  while (1)
    delay(1);
}

bool wifiHasIp() {
  bool r = wifiGotIpFlag;
  wifiGotIpFlag = false;
  return r;
}

// void wifiStartAp(const String &ssid) {
//   WiFiManager wifiManager;

//   if (!wifiManager.startConfigPortal(ssid.c_str())) {
//     logInfo("Failed to connect or setup, rebooting...");
//     delay(1000);
//     ESP.restart();
//     while (1)
//       delay(1);
//   }
// }

void wifiCallback(system_event_id_t evt, system_event_info_t info) {
  if (evt == SYSTEM_EVENT_STA_GOT_IP) {
    IPAddress addr(info.got_ip.ip_info.ip.addr);
    wifiGotIpFlag = true;
    logInfo("wifi: connected successfuly");
    logValue("wifi: got IP: ", addr);
  } else {
    logValue("wifi: event ",evt);
  }
}

void wifiSetup(const String &ssid) {
  if (WiFi.getAutoConnect())
    WiFi.setAutoConnect(false);
  if (WiFi.getMode() != WIFI_STA)
    WiFi.mode(WIFI_STA);

  logInfo("Connecting to WiFi...");
  wifiGotIpFlag = false;
  WiFi.onEvent(wifiCallback);

  // wifiIpHandler = WiFi.onStationModeGotIP([](const WiFiEventStationModeGotIP &evt) {
  //   // this executes when module reconnects and gets IP from DHCP
  //   // can be called multiple times
  //   wifiGotIpFlag = true;
  //   logInfo("WiFi connected successfuly");
  //   logValue("Got IP: ", evt.ip);
  // });

  if (WiFi.SSID() == "") {
    logInfo("No saved credentials");
    WiFi.begin(WIFI_SSID, WIFI_PASS);
  } else if (!WiFi.isConnected()) {
    logValue("Stored SSID: ", WiFi.SSID());
    WiFi.begin();
  }
}
