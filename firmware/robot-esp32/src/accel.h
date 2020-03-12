#pragma once

#include <Arduino.h>
#include <TaskSchedulerDeclarations.h>

bool accelSetup(int sda, int scl, Scheduler *scheduler);
void accelTick();
float accelGet();
