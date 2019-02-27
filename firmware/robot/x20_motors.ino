Task motorsTask(20, TASK_FOREVER, &motorsTick, &scheduler, true);

#define MOTORS_ACCEL_PER_TICK 20

uint8_t m1speed;
uint8_t m2speed;
uint8_t m1targetSpeed;
uint8_t m2targetSpeed;


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
    analogWrite(PIN_M1_EN, m1speed >= 128 ? map(m1speed, 128, 255, config.m1maxPwm / 4, config.m1maxPwm) : map(m1speed, 127, 0, config.m1maxPwm / 4, config.m1maxPwm));

  digitalWrite(PIN_M2_A, m2speed > 128);
  digitalWrite(PIN_M2_B, m2speed < 128);
  if (m2speed == 128)
    analogWrite(PIN_M2_EN, 0);
  else
    analogWrite(PIN_M2_EN, m2speed >= 128 ? map(m2speed, 128, 255, config.m2maxPwm / 4, config.m2maxPwm) : map(m2speed, 127, 0, config.m2maxPwm / 4, config.m2maxPwm));
}

void motorsSetup() {
  analogWriteFreq(20000);

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
