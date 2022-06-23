#pragma once

#include <Arduino.h>

#if USE_WIFI
void wifiSetup(const String &ssid);
void wifiResetAndRestart();
bool wifiHasIp();
#endif