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

uint8_t controlBuffer[32];

void controlProcessBuffer() {
  if (controlBuffer[0] != CONTROL_PACKET_MAGIC) {
    logValue("Control packet magic mismatch: ", controlBuffer[0]);
    return;
  }
  if (controlBuffer[1] == CONTROL_CMD_MOVE) {
    btnBrakeMask = controlBuffer[10];
    btnUpdateLeds();
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

  if (btnCodeToSend == 255) return;
  memset(controlBuffer, 0, sizeof(controlBuffer));
  len = 0;
  controlBuffer[len++] = CONTROL_PACKET_MAGIC;
  controlBuffer[len++] = CONTROL_CMD_REMOTE_CTRL;
  controlBuffer[len++] = btnCodeToSend;
  
  controlUdp.beginPacket(controlBroadcast, CONTROL_PORT);
  controlUdp.write(controlBuffer, len);
  controlUdp.endPacket();
  btnCodeToSend = 255;
}

void controlSetup() {
  controlUdp.begin(CONTROL_PORT);
  controlTask.enable();
}
