#include "RoboProtocol.h"


void protocolInit(ProtocolCmd cmd, void *buffer) {
  ProtocolCmdHeader *hdr = (ProtocolCmdHeader *)buffer;
  hdr->magic = PROTOCOL_PACKET_MAGIC;
  hdr->cmd = cmd;
}

ProtocolCmd protocolCheck(void *buffer, size_t len) {
  if (len < 2) {
    return ProtocolCmdInvalid;
  }

  uint8_t *buf = (uint8_t *)buffer;
  if (buf[0] != PROTOCOL_PACKET_MAGIC) {
    return ProtocolCmdInvalid;
  }
  switch (buf[1]) {
    case ProtocolCmdMove:
      return len == sizeof(ProtocolMsgMove) ? ProtocolCmdMove : ProtocolCmdInvalid;
    case ProtocolCmdVbat:
      return len == sizeof(ProtocolMsgVbat) ? ProtocolCmdVbat : ProtocolCmdInvalid;
    case ProtocolCmdRemoteCtrl:
      return len == sizeof(ProtocolMsgRemoteCtrl) ? ProtocolCmdRemoteCtrl : ProtocolCmdInvalid;
    default:
      return ProtocolCmdInvalid;
  }
}
