#ifndef __MESSAGE_H

#define MAX_DATA_SIZE (16*1024)			// MAX Data in messages

// Connection States
typedef enum {
	CONNECTION_STATE_INITIALIZED,
	CONNECTION_STATE_CONNECTING,
	CONNECTION_STATE_CONNECTED,
	CONNECTION_STATE_DISCONNECTING,
	CONNECTION_STATE_DISCONNECTED,
}CONNECTION_STATE;

// Message Types
typedef enum {
	MESSAGE_TYPE_COMMAND,
	MESSAGE_TYPE_RESULT,
}MESSAGE_TYPE;

// Message structure
typedef struct {
	MESSAGE_TYPE messageId;
	int length;
	char data[MAX_DATA_SIZE];
}message_t;

#endif
