#include "sender.h"
#include <log.h>
#include <RoboProtocol.h>
#if ESP32
#include <WiFi.h>
#include <esp_wifi.h>
#include <esp_now.h>
#else
#include <user_interface.h>
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

#define SENDER_MAX_LEN 32
struct SenderQueueItem {
  uint8_t mac[6];
  int len;
  uint8_t data[SENDER_MAX_LEN];
};

xQueueHandle senderQueue = NULL;

void espNowRecvCb(const uint8_t *mac, const uint8_t *data, int len) {
  SenderQueueItem item;

  if (len > SENDER_MAX_LEN || mac == NULL || data == NULL || len <= 0)
    return;

  memcpy(item.mac, mac, sizeof(item.mac));
  item.len = len;
  memcpy(item.data, data, len);
  xQueueSend(senderQueue, &item, 1024);
}

void senderTick() {
  SenderQueueItem item;
  while (xQueueReceive(senderQueue, &item, 0) == pdTRUE) {
    ProtocolCmd cmd = protocolCheck(item.data, item.len);
    if (cmd != ProtocolCmdInvalid && recvCallback) {
      memcpy(lastMac, item.mac, sizeof(item.mac));
      recvCallback(cmd, item.data, item.len);
    }
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

void senderTick() {}
#endif

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
  logValue("sender: setting radio to station and channel ", SENDER_CHANNEL);
#if ESP32
  WiFi.persistent(false);
  WiFi.begin("dummy", NULL, SENDER_CHANNEL, NULL, false);
  ESP_ERROR_CHECK(esp_wifi_set_channel(SENDER_CHANNEL, WIFI_SECOND_CHAN_NONE));
#else
  wifi_set_opmode_current(1); // station
  wifi_set_channel(SENDER_CHANNEL);
#endif
#endif

  if (esp_now_init()) {
    logInfo("sender: failed init");
  }
#if ESP32
  senderQueue = xQueueCreate(100, sizeof(SenderQueueItem));

  esp_now_peer_info_t peerInfo = {};
  memcpy(&peerInfo.peer_addr, broadcastMac, sizeof(broadcastMac));
  peerInfo.channel = SENDER_CHANNEL;
  peerInfo.ifidx = ESP_IF_WIFI_STA;
  peerInfo.encrypt = false;
  if (!esp_now_is_peer_exist(broadcastMac)) {
    esp_now_add_peer(&peerInfo);
  }
#else
  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
  if (!esp_now_is_peer_exist(broadcastMac)) {
    esp_now_add_peer(broadcastMac, ESP_NOW_ROLE_CONTROLLER, SENDER_CHANNEL, NULL, 0);
  }
#endif
  if (esp_now_register_recv_cb(espNowRecvCb)) {
    logInfo("sender: failed to register callback");
  }

  scheduler->addTask(senderTask);
  senderTask.enable();
  logInfo("sender: setup done");
}
