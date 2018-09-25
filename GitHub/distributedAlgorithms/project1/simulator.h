#ifndef SIMULATOR_H
#define SIMULATOR_H

#define THREAD_SLEEP_MICRO 10000

#include "messages.h"

// Node Statistics
typedef struct {
	int round;
	int numExploreMessages;
	int numAcks;
	int numNacks;
}Statistics;

// Node data
typedef struct {
	int nodeId;
	int uId;
	int parentId;
	int leaderId;
	int roundDone;
	int connected;
	int waitListCount;
	int recvBufferSize;
	int processBufferSize;

	pthread_mutex_t recvBufferMutex;
	pthread_mutex_t threadMutex;
	pthread_cond_t roundFinishCondition;
	pthread_t threadId;

	int *connectivity;
	int *children;
	message_t *recvBuffer; // queue of received messages
	message_t *processBuffer;

	Statistics stats;
}NodeState;

// Master notifications
typedef enum {
	START_ROUND = 0,
	END
}MASTER_NOTIFICATION;

// global state
typedef struct {
	int nProcess;
	int currentRound;
	int waitCount;

	NodeState **nodeStates;

	pthread_t masterThread;
	pthread_cond_t synchronyCondition;
	pthread_mutex_t masterMutex;

	MASTER_NOTIFICATION masterNotification;
}GlobalState;

// global variable for the state of the system
GlobalState globalState;


#endif
