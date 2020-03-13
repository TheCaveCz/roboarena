#pragma once

#include <Arduino.h>
#include <Motors.h>
#include <MP3Player.h>
#include <TaskSchedulerDeclarations.h>
#include <RoboProtocol.h>


bool logicSetup(Scheduler *scheduler, Motors *motors, MP3Player *player);
uint8_t logicGetUnitId();
void logicSetUnitId(uint8_t newId);
bool logicValidUnitId();
void logicRecv(ProtocolCmd cmd, void *buffer, size_t len);
uint32_t logicGetVbat();
void logicRunVbatCalib(bool run);
void logicSetBrake(bool brake, bool sound = true);
