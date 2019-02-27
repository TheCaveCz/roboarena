Task controlTask(20, TASK_FOREVER, &controlTick, &scheduler, false);
WiFiUDP controlUdp;

#define CONTROL_PACKET_MAGIC 0x62
#define CONTROL_CMD_MOVE 1
#define CONTROL_CMD_ID 2
#define CONTROL_CMD_MAXPWM 3

uint8_t controlBuffer[32];


void controlProcessBuffer() {
  if (controlBuffer[0] != CONTROL_PACKET_MAGIC) {
    logValue("Control packet magic mismatch: ", controlBuffer[0]);
    return;
  }
  if (controlBuffer[1] == CONTROL_CMD_MOVE) {
    uint8_t brakeMask = controlBuffer[2 + 8];

    if (brakeMask == 0xf) {
      pixelsSetColor(255, 0, 0);
    } else {
      pixelsSetColor(0, 255, 0);
    }
  }
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
