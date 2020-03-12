#pragma once

#include <Arduino.h>

void wifiSetup(const String &ssid);
void wifiResetAndRestart();
bool wifiHasIp();
