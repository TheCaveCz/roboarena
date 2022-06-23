#pragma once

#include <Arduino.h>
#include <RoboProtocol.h>
#include <TaskSchedulerDeclarations.h>
#include "control.h"


void senderReceive(ProtocolCmd cmd, const void *buffer, size_t len);
size_t senderSend(uint8_t id, void *buffer);
void buttonCallback(uint8_t buttonId, ButtonEventType ev);
void logicSetup(Scheduler *scheduler);
