#pragma once

#include <Arduino.h>
#include <TaskSchedulerDeclarations.h>
#include <RoboProtocol.h>

class IPAddress;

typedef void(*SenderRecvCallback)(ProtocolCmd cmd, void *buffer, size_t len);
typedef size_t(*SenderSendCallback)(uint8_t id, void *buffer);

void senderSetup(Scheduler *scheduler, SenderRecvCallback recvCallback, SenderSendCallback sendCallback);
void senderSendNow(const void *buffer, size_t len, IPAddress addr, uint16_t port);
void senderSendNow(const void *buffer, size_t len);

uint16_t senderGetPort();
IPAddress senderGetIp();
