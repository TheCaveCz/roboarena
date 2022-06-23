#pragma once

#include <Arduino.h>
#include <TaskSchedulerDeclarations.h>
#include <RoboProtocol.h>

#define SENDER_CHANNEL 11

typedef void (*SenderRecvCallback)(ProtocolCmd cmd, const void *buffer, size_t len);
typedef size_t (*SenderSendCallback)(uint8_t id, void *buffer);

void senderSetup(Scheduler *scheduler, SenderRecvCallback recvCallback, SenderSendCallback sendCallback);
void senderSendNow(void *buffer, size_t len, uint8_t *mac);
void senderSendNow(void *buffer, size_t len);
void senderTick();

uint8_t *senderGetMac();
