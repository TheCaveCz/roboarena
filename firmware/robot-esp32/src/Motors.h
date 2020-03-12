#pragma once

#include <Arduino.h>
#include <MiniPID.h>
#include <log.h>

class Motors;


class Motor {
  friend class Motors;

public:
  Motor(uint8_t id);

  void setSpeed(float speed);
  void setBrake(bool brake);
  void setMaxSpeed(float maxSpeed);

  void tick();

private:
  uint8_t _id;

  xQueueHandle _isrQueue;
  uint32_t _isrLastValue;

  float _targetSpeed;
  float _currentSpeed;
  float _rpm;
  float _maxSpeed;
  bool _brake;

  MiniPID _pid;

  void updateMotorPwm();
};


class Motors {
public:
  Motors();

  bool begin();

  void tick();

  void setSpeed(float m1speed, float m2speed);
  void setMaxSpeed(float maxSpeed);
  void setBrake(bool brake);

  void setP(double p);
  void setI(double i);
  void setD(double d);
  void setF(double f);
  void dumpPid() { logValue4("P:", _p, "I:", _i, "D:", _d, "F:", _f); }

private:
  static void isrHandler(void *arg);

  Motor _motor1;
  Motor _motor2;
  double _p;
  double _i;
  double _d;
  double _f;
};
