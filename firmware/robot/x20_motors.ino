Task motorsTask(20, TASK_FOREVER, &motorsTick, &scheduler, true);

#define MOTORS_ACCEL_PER_TICK 20

uint8_t m1speed;
uint8_t m2speed;
uint8_t m1targetSpeed;
uint8_t m2targetSpeed;

//#define PWM_1_MIN (config.m1maxPwm/4)
//#define PWM_2_MIN (config.m2maxPwm/4)

//#define PWM_1_MIN (config.m1maxPwm/8)
//#define PWM_2_MIN (config.m2maxPwm/8)

#define PWM_1_MIN 256
#define PWM_2_MIN 256

#define PWM_1_MAX (config.m1maxPwm)
#define PWM_2_MAX (config.m2maxPwm)


void motorsTick() {
  int delta = constrain((int)m1targetSpeed - (int)m1speed, -MOTORS_ACCEL_PER_TICK, MOTORS_ACCEL_PER_TICK);
  m1speed += delta;
  delta = constrain((int)m2targetSpeed - (int)m2speed, -MOTORS_ACCEL_PER_TICK, MOTORS_ACCEL_PER_TICK);
  m2speed += delta;

  digitalWrite(PIN_M1_A, m1speed > 128);
  digitalWrite(PIN_M1_B, m1speed < 128);
  if (m1speed == 128)
    analogWrite(PIN_M1_EN, 0);
  else
    analogWrite(PIN_M1_EN, m1speed >= 128 ? map(m1speed, 128, 255, PWM_1_MIN, PWM_1_MAX) : map(m1speed, 127, 0, PWM_1_MIN, PWM_1_MAX));

  digitalWrite(PIN_M2_A, m2speed > 128);
  digitalWrite(PIN_M2_B, m2speed < 128);
  if (m2speed == 128)
    analogWrite(PIN_M2_EN, 0);
  else
    analogWrite(PIN_M2_EN, m2speed >= 128 ? map(m2speed, 128, 255, PWM_2_MIN, PWM_2_MAX) : map(m2speed, 127, 0, PWM_2_MIN, PWM_2_MAX));
}

void motorsSetup() {
  analogWriteFreq(50);

  pinMode(PIN_M1_A, OUTPUT);
  pinMode(PIN_M1_B, OUTPUT);
  pinMode(PIN_M1_EN, OUTPUT);

  pinMode(PIN_M2_A, OUTPUT);
  pinMode(PIN_M2_B, OUTPUT);
  pinMode(PIN_M2_EN, OUTPUT);

  m1speed = 128;
  m1targetSpeed = 128;
  m2speed = 128;
  m2targetSpeed = 128;
  motorsTick();
}
