#include "log.h"

#if LOG_ENABLED
#if ARDUINO_SAMD_COREBOARD
Stream *logDebugSerial = &SerialUSB;
#else
Stream *logDebugSerial = &Serial;
#endif
#endif

void logDumpBuffer(void *abuffer, uint32_t size) {
#if LOG_ENABLED
  if (!logDebugSerial)
    return;

  if (size)
    logDebugSerial->write('#');

  uint8_t *buffer = (uint8_t *)abuffer;

  for (uint32_t i = 0; i < size; i++) {
    uint8_t data = buffer[i];
    if (data < 16)
      logDebugSerial->write('0');
    logDebugSerial->print(data, HEX);
    if (i % 4 == 3)
      logDebugSerial->write(' ');
    if (i % 32 == 31) {
      logDebugSerial->print(logF(" ;0x"));
      logDebugSerial->print(i - 31, HEX);
      logDebugSerial->println();
      logDebugSerial->write('#');
    }
  }

  if (size)
    logDebugSerial->println();
#endif
}
