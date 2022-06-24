#include "Motors.h"
#include <driver/mcpwm.h>
#include <soc/mcpwm_struct.h>
#include "pinmap.h"
#include <log.h>


#define CAP0_INT_EN BIT(27) // Capture 0 interrupt bit
#define CAP1_INT_EN BIT(28) // Capture 0 interrupt bit

#define MOTOR_GEAR_RATIO ((34.0f * 34.0f * 35.0f * 38.0f) / (13.0f * 12.0f * 13.0f * 10.0f))

struct MotorIsrData {
  uint32_t timestamp;
  uint8_t bInput;
};

#define SPEED_HW 900.0

Motor::Motor(uint8_t id)
    : _id(id)
    , _isrLastValue(0)
    , _targetSpeed(0)
    , _currentSpeed(0)
    , _rpm(0)
    , _maxSpeed(0.5)
    , _brake(false)
    , _pid(1.0, 0.55, 0.2, 0.0) {
  _isrQueue = xQueueCreate(256, sizeof(MotorIsrData));
  _pid.setOutputLimits(-SPEED_HW, SPEED_HW);
}

void Motor::setSpeed(float speed) {
  if (_brake)
    return;

  _targetSpeed = constrain(speed, -1.0f, 1.0f);
}

void Motor::setBrake(bool brake) {
  _brake = brake;
  if (brake) {
    _targetSpeed = 0;
  }
}

void Motor::setMaxSpeed(float maxSpeed) { _maxSpeed = constrain(maxSpeed, 0.1, 1); }

void Motor::updateMotorPwm() {
  float s = constrain(_currentSpeed, -1.0, 1.0);
  if (s > 0) {
    mcpwm_set_signal_high(MCPWM_UNIT_0, (mcpwm_timer_t)_id, MCPWM_OPR_B);
    mcpwm_set_duty(MCPWM_UNIT_0, (mcpwm_timer_t)_id, MCPWM_OPR_A, (1.0f - s) * 100.0f);
    mcpwm_set_duty_type(MCPWM_UNIT_0, (mcpwm_timer_t)_id, MCPWM_OPR_A, MCPWM_DUTY_MODE_0);
  } else if (s < 0) {
    mcpwm_set_signal_high(MCPWM_UNIT_0, (mcpwm_timer_t)_id, MCPWM_OPR_A);
    mcpwm_set_duty(MCPWM_UNIT_0, (mcpwm_timer_t)_id, MCPWM_OPR_B, (1.0f + s) * 100.0f);
    mcpwm_set_duty_type(MCPWM_UNIT_0, (mcpwm_timer_t)_id, MCPWM_OPR_B, MCPWM_DUTY_MODE_0);
  } else if (_brake) {
    mcpwm_set_signal_low(MCPWM_UNIT_0, (mcpwm_timer_t)_id, MCPWM_OPR_A);
    mcpwm_set_signal_low(MCPWM_UNIT_0, (mcpwm_timer_t)_id, MCPWM_OPR_B);
  } else {
    mcpwm_set_signal_high(MCPWM_UNIT_0, (mcpwm_timer_t)_id, MCPWM_OPR_A);
    mcpwm_set_signal_high(MCPWM_UNIT_0, (mcpwm_timer_t)_id, MCPWM_OPR_B);
  }
}

void Motor::tick() {
  MotorIsrData data;
  int32_t sum = 0;
  int32_t count = 0;
  int32_t val;
  while (xQueueReceive(_isrQueue, &data, 0) == pdTRUE) {
    val = (data.timestamp - _isrLastValue) / 80; // (dividing by APB freq in MHz to get uSec)
    if (data.bInput)
      val = -val;
    sum += val;
    count++;
    _isrLastValue = data.timestamp;
  }

  if (count) {
    _rpm = (60000000.0f / MOTOR_GEAR_RATIO) / ((float)(sum / count));
    if (_rpm < -2000 || _rpm > 2000) { // filter out excess rpm values to prevent oscillations
      _rpm = 0;
    }
  } else {
    _rpm = 0;
  }

  _currentSpeed = _pid.getOutput(_rpm, _targetSpeed * _maxSpeed * SPEED_HW) / SPEED_HW;
  updateMotorPwm();
}


Motors::Motors() : _motor1(0), _motor2(1) {}

void IRAM_ATTR Motors::isrHandler(void *arg) {
  Motors *motors = (Motors *)arg;

  uint32_t mcpwm_intr_status = MCPWM0.int_st.val;
  MotorIsrData data;
  if (mcpwm_intr_status & CAP0_INT_EN) {
    data.timestamp = mcpwm_capture_signal_get_value(MCPWM_UNIT_0, MCPWM_SELECT_CAP0);
    data.bInput = !digitalRead(PIN_M1_ENCB);
    xQueueSendFromISR(motors->_motor1._isrQueue, &data, NULL);
  }
  if (mcpwm_intr_status & CAP1_INT_EN) {
    data.timestamp = mcpwm_capture_signal_get_value(MCPWM_UNIT_0, MCPWM_SELECT_CAP1);
    data.bInput = digitalRead(PIN_M2_ENCB);
    xQueueSendFromISR(motors->_motor2._isrQueue, &data, NULL);
  }
  MCPWM0.int_clr.val = mcpwm_intr_status;
}

bool Motors::begin() {
  mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0A, PIN_M1A);
  mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0B, PIN_M1B);
  mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM1A, PIN_M2A);
  mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM1B, PIN_M2B);
  mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM_CAP_0, PIN_M1_ENCA);
  mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM_CAP_1, PIN_M2_ENCA);

  pinMode(PIN_M1_ENCB, INPUT);
  pinMode(PIN_M2_ENCB, INPUT);
  pinMode(PIN_MFAULT, INPUT);

  mcpwm_config_t config;
  config.frequency = 20000;
  config.cmpr_a = 0;
  config.cmpr_b = 0;
  config.duty_mode = MCPWM_DUTY_MODE_0;
  config.counter_mode = MCPWM_UP_COUNTER;
  if (mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_0, &config) != ESP_OK) {
    logInfo("mot: failed init t0");
    return false;
  }
  if (mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_1, &config) != ESP_OK) {
    logInfo("mot: failed init t1");
    return false;
  }

  mcpwm_capture_enable(MCPWM_UNIT_0, MCPWM_SELECT_CAP0, MCPWM_POS_EDGE, 0);
  mcpwm_capture_enable(MCPWM_UNIT_0, MCPWM_SELECT_CAP1, MCPWM_POS_EDGE, 0);

  MCPWM0.int_ena.cap0_int_ena = 1;
  MCPWM0.int_ena.cap1_int_ena = 1;

  mcpwm_isr_register(MCPWM_UNIT_0, isrHandler, this, 0, NULL);
  // for some reason mcpwm_capture_signal_get_value is not in IRAM, we need to call interrupt without IRAM flag
  // mcpwm_isr_register(MCPWM_UNIT_0, isr_handler, NULL, ESP_INTR_FLAG_IRAM, NULL);

  pinMode(PIN_MSLEEP, OUTPUT);
  digitalWrite(PIN_MSLEEP, HIGH);

  return true;
}

void Motors::setSpeed(float m1speed, float m2speed) {
  _motor1.setSpeed(m1speed);
  _motor2.setSpeed(m2speed);
}

void Motors::setMaxSpeed(float maxSpeed) {
  _motor1.setMaxSpeed(maxSpeed);
  _motor2.setMaxSpeed(maxSpeed);
}

void Motors::setBrake(bool brake) {
  _motor1.setBrake(brake);
  _motor2.setBrake(brake);
}

void Motors::tick() {
  _motor1.tick();
  _motor2.tick();

  // Serial.print(_motor1._targetSpeed*_motor1._maxSpeed*SPEED_HW);
  // Serial.print('\t');
  // Serial.print(_motor1._rpm);
  // Serial.print('\t');
  // Serial.print(_motor2._targetSpeed*_motor2._maxSpeed*SPEED_HW);
  // Serial.print('\t');
  // Serial.print(_motor2._rpm);
  // Serial.println();
}

void Motors::setP(double p) {
  _p = p;
  _motor1._pid.setP(p);
  _motor2._pid.setP(p);
}

void Motors::setI(double p) {
  _i = p;
  _motor1._pid.setI(p);
  _motor2._pid.setI(p);
}

void Motors::setD(double p) {
  _d = p;
  _motor1._pid.setD(p);
  _motor2._pid.setD(p);
}

void Motors::setF(double f) {
  _f = f;
  _motor1._pid.setF(f);
  _motor2._pid.setF(f);
}
