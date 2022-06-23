#pragma once

#include <Arduino.h>

#ifndef LOG_ENABLED
#define LOG_ENABLED 1
#endif

#if LOG_ENABLED

#ifndef LOG_USE_COLORS
#define LOG_USE_COLORS 1
#endif

extern Stream *logDebugSerial;

#define logRaw(msg) logDebugSerial->print(msg)
#define logRawEx(msg, fmt) logDebugSerial->print(msg, fmt)
#else
#define logRaw(msg)
#define logRawEx(msg, fmt)
#endif

#if __arm__
#define logF(msg) msg
#else
#define logF(msg) F(msg)
#endif

#define _logStr(B) #B
#define logStr(A) _logStr(A)

#if LOG_USE_COLORS
#define LOG_LINE "\x1b[0m\n"
#define LOG_HEADER "\x1b[32m#"
#else
#define LOG_LINE '\n'
#define LOG_HEADER '#'
#endif

#define logLine() logRaw(LOG_LINE)
#define logHeader()                                                                                                    \
  do {                                                                                                                 \
    logRaw(LOG_HEADER);                                                                                                \
    logRaw(millis());                                                                                                  \
    logRaw(" | ");                                                                                                     \
  } while (0)
#define logInfo(msg)                                                                                                   \
  do {                                                                                                                 \
    logHeader();                                                                                                       \
    logRaw(logF(msg));                                                                                                 \
    logLine();                                                                                                         \
  } while (0)
#define logValue(msg, val)                                                                                             \
  do {                                                                                                                 \
    logHeader();                                                                                                       \
    logRaw(logF(msg));                                                                                                 \
    logRaw(val);                                                                                                       \
    logLine();                                                                                                         \
  } while (0)
#define logValue2(msg, val, msg2, val2)                                                                                \
  do {                                                                                                                 \
    logHeader();                                                                                                       \
    logRaw(logF(msg));                                                                                                 \
    logRaw(val);                                                                                                       \
    logRaw(logF(", " msg2));                                                                                           \
    logRaw(val2);                                                                                                      \
    logLine();                                                                                                         \
  } while (0)
#define logValue3(msg, val, msg2, val2, msg3, val3)                                                                    \
  do {                                                                                                                 \
    logHeader();                                                                                                       \
    logRaw(logF(msg));                                                                                                 \
    logRaw(val);                                                                                                       \
    logRaw(logF(", " msg2));                                                                                           \
    logRaw(val2);                                                                                                      \
    logRaw(logF(", " msg3));                                                                                           \
    logRaw(val3);                                                                                                      \
    logLine();                                                                                                         \
  } while (0)
#define logValue4(msg, val, msg2, val2, msg3, val3, msg4, val4)                                                        \
  do {                                                                                                                 \
    logHeader();                                                                                                       \
    logRaw(logF(msg));                                                                                                 \
    logRaw(val);                                                                                                       \
    logRaw(logF(", " msg2));                                                                                           \
    logRaw(val2);                                                                                                      \
    logRaw(logF(", " msg3));                                                                                           \
    logRaw(val3);                                                                                                      \
    logRaw(logF(", " msg4));                                                                                           \
    logRaw(val4);                                                                                                      \
    logLine();                                                                                                         \
  } while (0)
#define logValue5(msg, val, msg2, val2, msg3, val3, msg4, val4, msg5, val5)                                            \
  do {                                                                                                                 \
    logHeader();                                                                                                       \
    logRaw(logF(msg));                                                                                                 \
    logRaw(val);                                                                                                       \
    logRaw(logF(", " msg2));                                                                                           \
    logRaw(val2);                                                                                                      \
    logRaw(logF(", " msg3));                                                                                           \
    logRaw(val3);                                                                                                      \
    logRaw(logF(", " msg4));                                                                                           \
    logRaw(val4);                                                                                                      \
    logRaw(logF(", " msg5));                                                                                           \
    logRaw(val5);                                                                                                      \
    logLine();                                                                                                         \
  } while (0)
#define logValue6(msg, val, msg2, val2, msg3, val3, msg4, val4, msg5, val5, msg6, val6)                                \
  do {                                                                                                                 \
    logHeader();                                                                                                       \
    logRaw(logF(msg));                                                                                                 \
    logRaw(val);                                                                                                       \
    logRaw(logF(", " msg2));                                                                                           \
    logRaw(val2);                                                                                                      \
    logRaw(logF(", " msg3));                                                                                           \
    logRaw(val3);                                                                                                      \
    logRaw(logF(", " msg4));                                                                                           \
    logRaw(val4);                                                                                                      \
    logRaw(logF(", " msg5));                                                                                           \
    logRaw(val5);                                                                                                      \
    logRaw(logF(", " msg6));                                                                                           \
    logRaw(val6);                                                                                                      \
    logLine();                                                                                                         \
  } while (0)

#define logValueEx(msg, val, fmt)                                                                                      \
  do {                                                                                                                 \
    logHeader();                                                                                                       \
    logRaw(logF(msg));                                                                                                 \
    logRawEx(val, fmt);                                                                                                \
    logLine();                                                                                                         \
  } while (0)

void logDumpBuffer(void *buffer, uint32_t size);
