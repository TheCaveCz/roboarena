#pragma once

#include <Arduino.h>
#include <RoboProtocol.h>


void senderReceive(ProtocolCmd cmd, void *buffer, size_t len);
size_t senderSend(uint8_t id, void *buffer);
void buttonCallback(uint8_t buttonId);
