#ifndef SIMULATOR_H
#define SIMULATOR_H

#define THREAD_SLEEP_MICRO 10000

#include "messages.h"

typedef struct {
	int round;
	int numExploreMessages;
	int numAcks;
	int numNacks;
	int numOtherMessages;
	int parentChange;
}Statistics;

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
	int *waitList;
	message_t *recvBuffer; // queue of received messages
	message_t *processBuffer;

	Statistics stats;
}NodeState;

typedef enum {
	START_ROUND = 0,
	END
}MASTER_NOTIFICATION;

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

GlobalState globalState;


#endif
