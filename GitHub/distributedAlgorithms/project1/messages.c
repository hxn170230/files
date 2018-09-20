#include "messages.h"

void logMessage(int nodeId, message_t message) {
        DEBUG("Node[%d]: Message(type:%d to:%d from: %d value: %d)\n", nodeId, message.type, message.toId, message.fromId, message.value);
}
