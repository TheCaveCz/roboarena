#include "sender.h"
#include <WiFiUdp.h>
#include <log.h>
#include <RoboProtocol.h>

WiFiUDP senderUdp;
IPAddress senderBroadcast(255, 255, 255, 255);
SenderRecvCallback recvCallback;
SenderSendCallback sendCallback;

void senderSendNow(const void *buffer, size_t len) {
  senderUdp.beginPacket(senderBroadcast, PROTOCOL_PORT);
  senderUdp.write((uint8_t*)buffer, len);
  senderUdp.endPacket();
}

void senderTick() {
  size_t len;
  uint8_t senderBuffer[32];

  if (senderUdp.parsePacket()) {
    memset(senderBuffer, 0, sizeof(senderBuffer));
    len = senderUdp.read(senderBuffer, sizeof(senderBuffer) - 1);
    ProtocolCmd cmd = protocolCheck(senderBuffer, len);
    if (cmd != ProtocolCmdInvalid && recvCallback) {
      recvCallback(cmd, senderBuffer, len);
    }
  }

  if (sendCallback) {
    uint8_t id = 0;
    while (1) {
      memset(senderBuffer, 0, sizeof(senderBuffer));
      len = sendCallback(id++, senderBuffer);
      if (len) {
        senderSendNow(senderBuffer, len);
      } else {
        break;
      }
    }
  }
}

Task senderTask(50, TASK_FOREVER, &senderTick);

void senderSetup(Scheduler *scheduler, SenderRecvCallback _rcb, SenderSendCallback _scb) {
  sendCallback = _scb;
  recvCallback = _rcb;
  senderUdp.begin(PROTOCOL_PORT);
  scheduler->addTask(senderTask);
  senderTask.enable();
}
