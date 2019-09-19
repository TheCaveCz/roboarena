Task controlTask(20, TASK_FOREVER, &controlTick, &scheduler, false);
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
  switch (controlBuffer[1]) {
    case CONTROL_CMD_MOVE: {
        if (!configIdValid()) return;

        // roboId ranges from 1 to 4 (incl.). this is assumed in following lines so that 2b header of packet can be skipped!
        uint8_t speedX = controlBuffer[config.roboId * 2];
        uint8_t speedY = controlBuffer[config.roboId * 2 + 1];
        uint8_t brakeMask = controlBuffer[2 + 8];

        logicSetSpeed(speedX, speedY);
        logicSetBrake(brakeMask & (1 << (config.roboId - 1)));

        break;
      }
    case CONTROL_CMD_MAXPWM: {
        config.m1maxPwm = ((uint16_t)controlBuffer[2] << 8) | (uint16_t)controlBuffer[3];
        config.m2maxPwm = ((uint16_t)controlBuffer[4] << 8) | (uint16_t)controlBuffer[5];
        configWrite();
        logInfo("Control set new pwm")
        logValue(" m1pwm: ", config.m1maxPwm);
        logValue(" m2pwm: ", config.m2maxPwm);
        break;
      }
    case CONTROL_CMD_ID: {
        config.roboId = controlBuffer[2];
        configWrite();
        logicUpdateRoboId();
        logValue("Control set new roboId: ", config.roboId);
        break;
      }
    case CONTROL_CMD_SET_LIFE: {
        if (!configIdValid()) return;
        if (controlBuffer[2] == config.roboId || controlBuffer[2] == 0) {
          logicSetLife(controlBuffer[3]);
        }
        break;
      }
    case CONTROL_CMD_DISCOVER: {
        uint8_t buf[8];
        buf[0] = CONTROL_PACKET_MAGIC;
        buf[1] = CONTROL_CMD_DISCOVER_RESPONSE;
        buf[2] = config.roboId;
        buf[3] = config.m1maxPwm >> 8;
        buf[4] = config.m1maxPwm & 0xff;
        buf[5] = config.m2maxPwm >> 8;
        buf[6] = config.m2maxPwm & 0xff;
        buf[7] = FW_VERSION;

        controlUdp.beginPacket(controlUdp.remoteIP(), controlUdp.remotePort());
        controlUdp.write(buf, sizeof(buf));
        controlUdp.endPacket();
        break;
      }
    default: {
        logValue("Control unknown command: ", controlBuffer[1]);
        break;
      }
  }
}

void controlSendHit(const uint8_t enemyId, const uint8_t life) {
  uint8_t buf[5];
  buf[0] = CONTROL_PACKET_MAGIC;
  buf[1] = CONTROL_CMD_HIT;
  buf[2] = config.roboId;
  buf[3] = enemyId;
  buf[4] = life;

  controlUdp.beginPacket(controlBroadcast, CONTROL_PORT);
  controlUdp.write(buf, sizeof(buf));
  controlUdp.endPacket();
}

void controlSendBrakeRequest() {
  if (!configIdValid()) return;

  uint8_t buf[3];
  buf[0] = CONTROL_PACKET_MAGIC;
  buf[1] = CONTROL_CMD_REMOTE_CTRL;
  buf[2] = config.roboId - 1;

  controlUdp.beginPacket(controlBroadcast, CONTROL_PORT);
  controlUdp.write(buf, sizeof(buf));
  controlUdp.endPacket();
}

void controlTick() {
  int len = controlUdp.parsePacket();
  if (len == 0) return;

  memset(controlBuffer, 0, sizeof(controlBuffer));
  len = controlUdp.read(controlBuffer, sizeof(controlBuffer) - 1);
  if (len <= 0) return;
  controlProcessBuffer();
}

void controlSetup() {
  controlUdp.begin(44123);
  controlTask.enable();
}
