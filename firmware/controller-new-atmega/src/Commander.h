#pragma once

#include <Arduino.h>

//
// Helper class for handling serial commands
//

#define COMMAND(__name) void __name(__attribute__((unused)) const char *buf, __attribute__((unused)) const uint8_t len)

typedef struct {
  char code;
  uint8_t len;
  void (*callback)(const char *cmd, const uint8_t len);
} Command;

#define CMD_OK 0
#define CMD_ERROR 1
#define CMD_SILENT 255

class Commander {
public:
  Commander();

  void begin(const Command *commands, Stream *_serial);
  void check();

  void printError();
  void printOk();

  bool checkHex(const uint8_t val);
  bool checkHex(const char *buf, const uint8_t from, const uint8_t to);
  uint16_t parseHexWord(const char *buf, const uint8_t from);
  uint8_t parseHex(const uint8_t v1, const uint8_t v2);
  uint8_t parseHex(const uint8_t val);
  bool checkRange(const uint8_t val, const uint8_t minVal, const uint8_t maxVal);
  void printHex(const uint8_t b);
  void printHexWord(const uint16_t w);
  void printCommands();

private:
  char _buf[32];
  uint8_t _pos;
  const Command *_cmds;
  uint8_t _of;
  Stream *_serial;

  void exec();
};
