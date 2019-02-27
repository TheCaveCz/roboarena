Task txTask(50, TASK_FOREVER, &txCb, &scheduler, true);

void txSetup() {
  Serial.begin(115200);
}

void txWriteHex(const uint8_t hex) {
  if (hex < 16) Serial.write('0');
  Serial.print(hex, HEX);
}

void txReadCmd() {
  int cmd = Serial.read();
  if (Serial.read() != 10) return;
  switch (cmd) {
    case 'C':
      btnConnectFlag = 1;
      btnUpdateLeds();
      break;
    case 'D':
      btnConnectFlag = 0;
      btnUpdateLeds();
      break;
    case 'X':
      btnResetFlag = 0;
      break;

    case '0':
    case '1':
    case '2':
    case '3':
      btnBrakeMask |= 1 << (cmd - '0');
      btnUpdateLed(cmd - '0');
      break;

    case '5':
    case '6':
    case '7':
    case '8':
      btnBrakeMask &= ~(1 << (cmd - '5'));
      btnUpdateLed(cmd - '5');
      break;

    case '4':
      btnStartAllClick();
      break;
    case '9':
      btnStopAllClick();
      break;
    default:
      break;
  }
}

void txCb() {
  Serial.write('R');
  for (uint8_t i = 0; i < ROBOT_COUNT * 2; i++) {
    txWriteHex(adcValues[i]);
  }
  Serial.print(btnBrakeMask & 0xf, HEX);
  Serial.write(btnResetFlag ? '1' : '0');
  Serial.write(10);

  while (Serial.available() > 2) {
    if (Serial.read() == 'R') {
      txReadCmd();
    }
  }
}
