#pragma once
#if USE_WIFI
#include <Arduino.h>

void wifiSetup(const String &ssid);
void wifiResetAndRestart();
bool wifiHasIp();
#endif