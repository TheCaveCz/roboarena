#pragma once

#include <Arduino.h>


#define PROTOCOL_PORT 44123
#define PROTOCOL_PACKET_MAGIC 0x63
// #define CONTROL_CMD_MOVE 1
// #define CONTROL_CMD_ID 2
// #define CONTROL_CMD_MAXPWM 3
// #define CONTROL_CMD_DISCOVER 4
// #define CONTROL_CMD_DISCOVER_RESPONSE 5
// #define CONTROL_CMD_REMOTE_CTRL 6
// #define CONTROL_CMD_SET_LIFE 7
// #define CONTROL_CMD_HIT 8

enum ProtocolCmd : uint8_t {
  ProtocolCmdInvalid = 0,
  ProtocolCmdMove = 1,
};

struct __attribute__((packed)) ProtocolCmdHeader {
  uint8_t magic;
  ProtocolCmd cmd;
};

struct __attribute__((packed)) ProtocolMsgMove {
  ProtocolCmdHeader header;
  uint8_t speeds[8];
  uint8_t brakeMask;
};

void protocolInit(ProtocolCmd cmd, void *buffer);
ProtocolCmd protocolCheck(void *buffer, size_t len);