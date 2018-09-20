#ifndef MESSAGES_H
#define MESSAGES_H

#include "log.h"

typedef enum {
	MESSAGE_TYPE_EXPLORE = 0,
	MESSAGE_TYPE_ACK,
	MESSAGE_TYPE_NACK,
	MESSAGE_TYPE_NONE, // used for null

	MESSAGE_TYPE_MAX
}MESSAGE_TYPE;

typedef struct {
	MESSAGE_TYPE type;
	// TODO extend this to void pointer
	int value;
	int fromId;
	int toId;
}message_t;

void logMessage(int nodeId, message_t message);

#endif
