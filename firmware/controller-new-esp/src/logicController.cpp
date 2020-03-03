#if LOGIC_CONTROLLER
#include "logic.h"
#include "control.h"
#include "wifi.h"

void senderReceive(ProtocolCmd cmd, void *buffer, size_t len) {}

size_t senderSend(uint8_t id, void *buffer) {
  if (id == 0) {
    ProtocolMsgMove msg;
    protocolInit(ProtocolCmdMove, &msg);
    memcpy(msg.speeds, controlGetPos(), sizeof(msg.speeds));
    msg.brakeMask = controlGetBrakeMask();
    memcpy(buffer, &msg, sizeof(msg));
    return sizeof(msg);
  } else {
    return 0;
  }
}

void buttonCallback(uint8_t buttonId) {
  if (buttonId == 6) {
    wifiResetAndRestart();
  }
}
#endif