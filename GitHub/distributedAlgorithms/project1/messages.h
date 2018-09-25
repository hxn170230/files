#ifndef MESSAGES_H
#define MESSAGES_H

#include "log.h"

// messages to send/receive
typedef enum {
	MESSAGE_TYPE_NONE = 0, // used for null message
	MESSAGE_TYPE_EXPLORE,
	MESSAGE_TYPE_ACK,
	MESSAGE_TYPE_NACK,
	MESSAGE_TYPE_NACK_EXPLORE,

	MESSAGE_TYPE_MAX
}MESSAGE_TYPE;

// structure of the message
typedef struct {
	MESSAGE_TYPE type;
	// TODO extend this to void pointer
	int value;
	int fromId;
	int toId;
}message_t;

// logs the message for node id
void logMessage(int nodeId, message_t message);

#endif
