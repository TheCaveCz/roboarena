#include <Arduino.h>
#include <TaskScheduler.h>
#include <OneButton.h>
#include "pinmap.h"
#include "Commander.h"


#define controlStream Serial1

OneButton btn1(PIN_BTN1, 1);
OneButton btn2(PIN_BTN2, 1);
OneButton btn3(PIN_BTN3, 1);
OneButton btn4(PIN_BTN4, 1);
OneButton btn5(PIN_BTN5, 1);
OneButton btn6(PIN_BTN6, 1);
Scheduler scheduler;
Commander cmd;

int adcBases[8];
const uint8_t adcPins[8] = {PIN_C1X, PIN_C1Y, PIN_C2X, PIN_C2Y, PIN_C3X, PIN_C3Y, PIN_C4X, PIN_C4Y};

void writeHex(uint8_t v) {
  if (v < 16)
    controlStream.write('0');
  controlStream.print(v, HEX);
}

int adcRemap(const int val, const int base) {
  if (val < 16)
    return 0;
  else if (val < base)
    return map(val, 16, base, 0, 128);
  else if (val > 1023-16)
    return 255;
  else
    return map(val, base, 1023-16, 128, 255);
}

void adcCallback() {
  uint8_t adcValues[8];
  int ar;
  for (uint8_t i = 0; i < 8; i++) {
    ar = analogRead(adcPins[i]);
    adcValues[i] = adcRemap(ar, adcBases[i]);
  }

  controlStream.write('Z');
  for (uint8_t i = 0; i < 8; i++) {
    writeHex(adcValues[i]);
  }
  controlStream.write('\n');
}

Task adcTask(50, TASK_FOREVER, &adcCallback, &scheduler);

void adcCalibrate() {
  for (uint8_t i = 0; i < 8; i++) {
    adcBases[i] = analogRead(adcPins[i]);
  }
}

void btnClick(void *param) {
  int id = (int)param;
  controlStream.write('X');
  controlStream.write(id + '0');
  controlStream.write('\n');
}

void btnLongStart(void *param) {
  int id = (int)param;
  controlStream.write('Y');
  controlStream.write(id + '0');
  controlStream.write('\n');
}

void btnLongStop(void *param) {
  int id = (int)param;
  controlStream.write('y');
  controlStream.write(id + '0');
  controlStream.write('\n');
}

COMMAND(cmdLeds) {
  uint8_t led = cmd.parseHex(buf[1]);
  digitalWrite(PIN_LED1, led & 1);
  digitalWrite(PIN_LED2, led & 2);
  digitalWrite(PIN_LED3, led & 4);
  digitalWrite(PIN_LED4, led & 8);
}

COMMAND(cmdReport) {
  uint8_t en = cmd.parseHex(buf[1]);
  if (en) {
    adcTask.enableIfNot();
  } else {
    adcTask.disable();
  }
}

COMMAND(cmdCalib) {
  if (buf[1] == '^' && buf[2] == '!') {
    adcCalibrate();
  }
}

const Command commands[] = {
    {'L', 2, cmdLeds},   //
    {'R', 2, cmdReport}, //
    {'X', 3, cmdCalib},  //
    {0, 0, NULL}         //
};

void setup() {
  Serial.begin(57600);
  Serial.println();

  controlStream.begin(19200);
  controlStream.println();

  pinMode(PIN_LED1, OUTPUT);
  pinMode(PIN_LED2, OUTPUT);
  pinMode(PIN_LED3, OUTPUT);
  pinMode(PIN_LED4, OUTPUT);
  digitalWrite(PIN_LED1, 0);
  digitalWrite(PIN_LED2, 0);
  digitalWrite(PIN_LED3, 0);
  digitalWrite(PIN_LED4, 0);

  btn1.attachClick(btnClick, (void *)1);
  btn2.attachClick(btnClick, (void *)2);
  btn3.attachClick(btnClick, (void *)3);
  btn4.attachClick(btnClick, (void *)4);
  btn5.attachClick(btnClick, (void *)5);
  btn6.attachClick(btnClick, (void *)6);

  btn1.setPressTicks(3000);
  btn2.setPressTicks(3000);
  btn3.setPressTicks(3000);
  btn4.setPressTicks(3000);
  btn5.setPressTicks(3000);
  btn6.setPressTicks(3000);

  btn1.attachLongPressStart(btnLongStart, (void *)1);
  btn2.attachLongPressStart(btnLongStart, (void *)2);
  btn3.attachLongPressStart(btnLongStart, (void *)3);
  btn4.attachLongPressStart(btnLongStart, (void *)4);
  btn5.attachLongPressStart(btnLongStart, (void *)5);
  btn6.attachLongPressStart(btnLongStart, (void *)6);

  btn1.attachLongPressStop(btnLongStop, (void *)1);
  btn2.attachLongPressStop(btnLongStop, (void *)2);
  btn3.attachLongPressStop(btnLongStop, (void *)3);
  btn4.attachLongPressStop(btnLongStop, (void *)4);
  btn5.attachLongPressStop(btnLongStop, (void *)5);
  btn6.attachLongPressStop(btnLongStop, (void *)6);

  adcCalibrate();
  adcTask.enable();

  cmd.begin(commands, &controlStream);
}

void loop() {
  btn1.tick();
  btn2.tick();
  btn3.tick();
  btn4.tick();
  btn5.tick();
  btn6.tick();

  scheduler.execute();
  cmd.check();
}