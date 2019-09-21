// expecting message of 14 bytes @ 9600 baud, so 20ms for timeout should be enough
Task taskReader(5, TASK_FOREVER, &readerMsgStartCb, &scheduler, true);

#define READER_ERROR_TOO_SHORT 1
#define READER_ERROR_INVALID_DATA 2
#define READER_ERROR_CHECKSUM 3

SoftwareSerial readerSerial(PIN_READER, -1);

#define READER_BINARY_CHECKSUM 1

#if READER_BINARY_CHECKSUM
#define READER_MSG_LEN 13
#else
#define READER_MSG_LEN 14
#endif

void readerChipEvent(const uint32_t chip);

void readerSetup() {
  readerSerial.begin(9600);
}

void readerMsgStartCb() {
  if (readerSerial.available() < READER_MSG_LEN) {
    return;
  }

  // discard all received data until we find sequence start marker
  while (readerSerial.available()) {
    Serial.write(readerSerial.peek());
    if (readerSerial.read() != 2) continue;

    uint32_t code = 0;
    uint8_t error = readerParseMessage(code);
    if (error == 0) {
      //logValue("Chip: ", code);
      if (code) {
        readerChipEvent(code);
      }
    }
    //    else {
    //      logValue("Reader error: ", error);
    //    }
    break;
  }
}

uint8_t readerConvertHex(uint8_t c) {
  switch (c) {
    case '0'...'9': return c - '0';
    case 'a'...'f': return c - 'a' + 10;
    case 'A'...'F': return c - 'A' + 10;
    default: return 255;
  }
}

bool readerGetByte(uint8_t &c) {
  uint8_t c1 = readerConvertHex(readerSerial.read());
  uint8_t c2 = readerConvertHex(readerSerial.read());
  if (c1 == 255 || c2 == 255) return false;

  c = (c1 << 4) | c2;
  return true;
}

uint8_t readerParseMessage(uint32_t& message) {
  if (readerSerial.available() < READER_MSG_LEN - 1)
    return READER_ERROR_TOO_SHORT;

  uint8_t c = 0;
  if (!readerGetByte(c)) // read first two chars - vendor id, used only in checksum calc
    return READER_ERROR_INVALID_DATA;

  uint8_t checksum = c;
  uint32_t data = 0;
  for (uint8_t i = 0; i < 4; i++) {
    if (!readerGetByte(c))
      return READER_ERROR_INVALID_DATA;
    data <<= 8;
    data |= c;
    checksum ^= c;
  }

#if READER_BINARY_CHECKSUM
  c = readerSerial.read();
#else
  if (!readerGetByte(c)) // read last two chars - checksum
    return READER_ERROR_INVALID_DATA;
#endif

  if (c != checksum) return READER_ERROR_CHECKSUM;

  if (readerSerial.read() != 3) return READER_ERROR_INVALID_DATA;

  message = data;
  return 0;
}
