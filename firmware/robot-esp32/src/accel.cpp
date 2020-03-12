#include "accel.h"

#include <Wire.h>
#include <SparkFun_MMA8452Q.h>
#include <log.h>

MMA8452Q accel;
float accelSum;
float accelAvg;
int accelCount;

void accelTick() {
  if (!accel.available())
    return;

  float x = accel.getCalculatedX();
  float y = accel.getCalculatedY();
  float a = sqrtf(powf(x, 2) + powf(y, 2));

  accelSum += a;
  accelCount++;
}

float accelGet() { return accelAvg; }

void accelCb() {
  if (accelCount == 0) {
    accelAvg = 0;
  } else {
    accelAvg = accelSum / ((float)accelCount);
  }
  accelSum = 0;
  accelCount = 0;
}
Task accelTask(25, TASK_FOREVER, &accelCb);

bool accelSetup(int sda, int scl, Scheduler *scheduler) {
  if (!Wire.begin(sda, scl)) {
    logInfo("i2c: failed to begin");
    return false;
  }

  if (!accel.begin(Wire, 0x1c)) {
    logInfo("accel: failed to begin");
    return false;
  }
  accel.setScale(SCALE_4G);

  scheduler->addTask(accelTask);
  accelTask.enable();

  return true;
}
