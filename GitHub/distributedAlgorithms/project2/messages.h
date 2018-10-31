#ifndef MESSAGES_H
#define MESSAGES_H

#include "log.h"

typedef struct {
	int id1;
	int id2;
	int edgeWeight;
}MWOEData;

typedef struct {
	int level;
	int componentId;
}AlgorithmData;

// messages to send/receive
typedef enum {
	MESSAGE_TYPE_NONE = 0, // used for null message
	MESSAGE_TYPE_BROADCAST,
	MESSAGE_TYPE_TESTMWOE,
	MESSAGE_TYPE_TESTMWOE_RESP,
	MESSAGE_TYPE_MWOEDATA,
	MESSAGE_TYPE_MWOE_START_CONNECT,
	MESSAGE_TYPE_MWOECONNECT,
	MESSAGE_TYPE_MWOECONNECT_RESP,

	MESSAGE_TYPE_MAX
}MESSAGE_TYPE;

// structure of the message
typedef struct {
	MESSAGE_TYPE type;
	// TODO extend this to void pointer
	int value;
	int fromId;
	int toId;
	int delay;
	int uId;

	AlgorithmData algoData;
	MWOEData data;
}message_t;

// logs the message for node id
void logMessage(int nodeId, message_t message);

#endif
