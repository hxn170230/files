#include "messages.h"

// logs the message
void logMessage(int nodeId, message_t message) {
        DEBUG("Node[%d]: Message(type:%d to:%d from: %d value: %d delay: %d)\n", nodeId, message.type, message.toId, message.fromId, message.value, message.delay);
}

void logSendMessage(int nodeId, message_t message) {
        DEBUG("Node[%d]: Send Message(type:%d to:%d from: %d value: %d delay: %d uId: %d level: %d component: %d)\n",
			nodeId, message.type, message.toId, message.fromId, message.value, message.delay,
			message.uId, message.algoData.level, message.algoData.componentId);
}

void logRecvMessage(int nodeId, message_t message) {
        DEBUG("Node[%d]: Recv Message(type:%d to:%d from: %d value: %d delay: %d uId: %d level: %d component: %d)\n",
			nodeId, message.type, message.toId, message.fromId, message.value, message.delay,
			message.uId, message.algoData.level, message.algoData.componentId);
}
