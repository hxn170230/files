#ifndef _CLIENT_H

#include "message.h"

// client global state
typedef struct {
	int serverFd;
	int logFd;
	int clientId;
	CONNECTION_STATE clientState;
}client_t;

#endif
