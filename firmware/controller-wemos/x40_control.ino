Task controlTask(50, TASK_FOREVER, &controlTick, &scheduler, false);
WiFiUDP controlUdp;
IPAddress controlBroadcast(255, 255, 255, 255);

#define CONTROL_PORT 44123
#define CONTROL_PACKET_MAGIC 0x62
#define CONTROL_CMD_MOVE 1
#define CONTROL_CMD_ID 2
#define CONTROL_CMD_MAXPWM 3
#define CONTROL_CMD_DISCOVER 4
#define CONTROL_CMD_DISCOVER_RESPONSE 5
#define CONTROL_CMD_REMOTE_CTRL 6
#define CONTROL_CMD_SET_LIFE 7
#define CONTROL_CMD_HIT 8

uint8_t controlBuffer[32];

void controlProcessBuffer() {
  if (controlBuffer[0] != CONTROL_PACKET_MAGIC) {
    logValue("Control packet magic mismatch: ", controlBuffer[0]);
    return;
  }
  if (controlBuffer[1] == CONTROL_CMD_REMOTE_CTRL) {
    uint8_t btn = controlBuffer[2];
    if (btn >= 10) {
      logValue("Button ID out of range: ", btn);
      return;
    }
    Serial.write('R');
    Serial.write(btn + '0');
    Serial.write(10);
  }
}

void controlTick() {
  int len;

  if (controlUdp.parsePacket()) {
    memset(controlBuffer, 0, sizeof(controlBuffer));
    len = controlUdp.read(controlBuffer, sizeof(controlBuffer) - 1);
    if (len > 0) {
      controlProcessBuffer();
    }
  }

  memset(controlBuffer, 0, sizeof(controlBuffer));
  len = 0;
  controlBuffer[len++] = CONTROL_PACKET_MAGIC;
  controlBuffer[len++] = CONTROL_CMD_MOVE;
  for (uint8_t i = 0; i < 8; i++) {
    controlBuffer[len++] = parserSpeeds[i];
  }
  controlBuffer[len++] = parserBrakeMask & 0xf;

  controlUdp.beginPacket(controlBroadcast, CONTROL_PORT);
  controlUdp.write(controlBuffer, len);
  controlUdp.endPacket();
}

void controlSetup() {
  controlUdp.begin(CONTROL_PORT);
  controlTask.enable();
}
