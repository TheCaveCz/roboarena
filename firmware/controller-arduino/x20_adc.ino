Task adcTask(20, TASK_FOREVER, &adcSampleCb, &scheduler, true);

// X1,Y1,X2,Y2,X3,Y3,X4,Y4
const uint8_t adcPins[ROBOT_COUNT * 2] = {A6, A7, A4, A5, A2, A3, A0, A1};
int adcBases[ROBOT_COUNT * 2];
uint8_t adcValues[ROBOT_COUNT * 2];


int adcRemap(const int val, const int base) {
  if (val < 0)
    return map(val, -base, 0, 0, 128);
  else
    return map(val, 0, 1023 - base, 128, 255);
}

void adcSampleCb() {
  for (uint8_t i = 0; i < ROBOT_COUNT * 2; i++) {
    adcValues[i] = adcRemap(analogRead(adcPins[i]) - adcBases[i], adcBases[i]);
  }
}

void adcCalibrate() {
  for (uint8_t i = 0; i < ROBOT_COUNT * 2; i++) {
    adcBases[i] = analogRead(adcPins[i]);
  }
}

void adcSetup() {
  adcCalibrate();
}
