#ifndef _SERVER_H

#include "message.h"

#define MAX_CLIENTS 10				// Max clients
#define SERVER_LOG_FILE "ServerLog.txt"		// Server Log
#define BACK_LOG 10				// Max simultaneous connection requests

// Client states
typedef struct {
	CONNECTION_STATE state;
	int clientFd;
	pthread_t threadId;
}client_t;

// server global state
typedef struct {
	int serverFd;
	client_t clients[MAX_CLIENTS];
	int logFd;
}server_t;

#endif
