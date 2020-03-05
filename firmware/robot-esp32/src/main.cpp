#include <Arduino.h>
#include <SPIFFS.h>
#include <log.h>
#include <SPI.h>
#include "MP3Player.h"

MP3Player player;

void setup() {
  delay(2000);

  Serial.begin(115200);
  logInfo("Robot!");

  if (!SPIFFS.begin()) {
    logInfo("spiffs: failed to begin");
    return;
  }

  if (!player.begin(33, 25, 32)) {
    logInfo("Player failed");
    return;
  }
  //player.play("/dead-1.mp3");
}

void loop() {
  player.loop();

  if (Serial.available()) {
    String file = "/";
    file += Serial.readStringUntil('\n');
    file.trim();
    file += ".mp3";
    player.play(file);
  }
}