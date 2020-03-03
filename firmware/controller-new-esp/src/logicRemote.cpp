#if LOGIC_REMOTE
#include "logic.h"


void senderReceive(ProtocolCmd cmd, void *buffer, size_t len) {}

size_t senderSend(uint8_t id, void *buffer) { return 0; }

void buttonCallback(uint8_t buttonId) {}

#endif