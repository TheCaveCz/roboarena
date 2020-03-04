#pragma once

#include <Arduino.h>


#define PROTOCOL_PORT 44123
#define PROTOCOL_PACKET_MAGIC 0x63
// #define CONTROL_CMD_ID 2
// #define CONTROL_CMD_MAXPWM 3
// #define CONTROL_CMD_DISCOVER 4
// #define CONTROL_CMD_DISCOVER_RESPONSE 5
// #define CONTROL_CMD_SET_LIFE 7
// #define CONTROL_CMD_HIT 8

enum ProtocolCmd : uint8_t {
  ProtocolCmdInvalid = 0,
  ProtocolCmdMove = 1,
  ProtocolCmdVbat = 2,
  ProtocolCmdRemoteCtrl = 6,
};

enum ProtocolRemoteCommand : uint8_t {
  ProtocolRemoteCommandToggleBrake1 = 1,
  ProtocolRemoteCommandToggleBrake2 = 2,
  ProtocolRemoteCommandToggleBrake3 = 3,
  ProtocolRemoteCommandToggleBrake4 = 4,
  ProtocolRemoteCommandStartAll = 5,
  ProtocolRemoteCommandStopAll = 6,
  ProtocolRemoteCommandCalibrate = 7,
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

struct __attribute__((packed)) ProtocolMsgVbat {
  ProtocolCmdHeader header;
  uint8_t unitId;
  uint16_t vbat;
};

struct __attribute__((packed)) ProtocolMsgRemoteCtrl {
  ProtocolCmdHeader header;
  ProtocolRemoteCommand command;
};


void protocolInit(ProtocolCmd cmd, void *buffer);
ProtocolCmd protocolCheck(void *buffer, size_t len);