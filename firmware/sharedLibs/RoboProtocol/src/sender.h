#pragma once

#include <Arduino.h>
#include <TaskSchedulerDeclarations.h>
#include <RoboProtocol.h>

typedef void(*SenderRecvCallback)(ProtocolCmd cmd, void *buffer, size_t len);
typedef size_t(*SenderSendCallback)(uint8_t id, void *buffer);

void senderSetup(Scheduler *scheduler, SenderRecvCallback recvCallback, SenderSendCallback sendCallback);
void senderSendNow(const void *buffer, size_t len);
