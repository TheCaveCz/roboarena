#include "sender.h"
#include <WiFiUdp.h>
#include <log.h>
#include <RoboProtocol.h>
#if ESP32
#include <esp_now.h>
#else
#include <espnow.h>
#endif

SenderRecvCallback recvCallback;
SenderSendCallback sendCallback;

static uint8_t lastMac[6] = {0, 0, 0, 0, 0, 0};
static uint8_t broadcastMac[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

void senderSendNow(void *buffer, size_t len, uint8_t *mac) { esp_now_send(mac, (uint8_t *)buffer, len); }

void senderSendNow(void *buffer, size_t len) { senderSendNow(buffer, len, broadcastMac); }

uint8_t *senderGetMac() { return lastMac; }

#if ESP32
void espNowRecvCb(const uint8_t *mac, const uint8_t *data, int len) {
  memcpy(lastMac, mac, sizeof(lastMac));
  ProtocolCmd cmd = protocolCheck(data, len);
  if (cmd != ProtocolCmdInvalid && recvCallback) {
    recvCallback(cmd, data, len);
  }
}
#else
void espNowRecvCb(uint8_t *mac, uint8_t *data, uint8_t len) {
  memcpy(lastMac, mac, sizeof(lastMac));
  ProtocolCmd cmd = protocolCheck(data, len);
  if (cmd != ProtocolCmdInvalid && recvCallback) {
    recvCallback(cmd, data, len);
  }
}
#endif


void senderTick() {}

void senderCb() {
  if (sendCallback == NULL)
    return;

  size_t len;
  uint8_t senderBuffer[32];

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

Task senderTask(50, TASK_FOREVER, &senderCb);

void senderSetup(Scheduler *scheduler, SenderRecvCallback _rcb, SenderSendCallback _scb) {
  logInfo("sender: starting");
  sendCallback = _scb;
  recvCallback = _rcb;

#if !USE_WIFI
#if ESP32
#else
  logValue("sender: setting radio to station and channel ", SENDER_CHANNEL);
  wifi_set_opmode_current(1); // station
  wifi_set_channel(SENDER_CHANNEL);
#endif
#endif

  if (esp_now_init()) {
    logInfo("sender: failed init");
  }
#if ESP32
  esp_now_peer_info_t peerInfo = {};
  memcpy(&peerInfo.peer_addr, broadcastMac, sizeof(broadcastMac));
  peerInfo.chanel = SENDER_CHANNEL;
  if (!esp_now_is_peer_exist(broadcastMac)) {
    esp_now_add_peer(&peerInfo);
  }
#else
  if (esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER)) {
    logInfo("sender: set role failed");
  }

  if (esp_now_is_peer_exist(broadcastMac)) {
    logInfo("sender: broadcast peer exists");
  } else {
    if (esp_now_add_peer(broadcastMac, ESP_NOW_ROLE_CONTROLLER, SENDER_CHANNEL, NULL, 0)) {
      logInfo("sender: failed to add peer");
    }
  }
#endif
  if (esp_now_register_recv_cb(espNowRecvCb)) {
    logInfo("sender: failed to register callback");
  }

  scheduler->addTask(senderTask);
  senderTask.enable();
  logInfo("sender: setup done");
}
