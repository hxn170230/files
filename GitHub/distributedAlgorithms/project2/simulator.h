#ifndef SIMULATOR_H
#define SIMULATOR_H

#define THREAD_SLEEP_MICRO 10000
#define MAX_QUEUE_SIZE 10000
#define SUCCESS 1
#define FAILURE 0

#define MAX_RAND_RANGE 20
#define MAX_ROUNDS 20
#define MAX_INT 2147483647

#include "messages.h"

typedef enum {
	BASIC_EDGE = 0,
	TREE_EDGE,
	REJECTED_EDGE,
}EdgeStatus_T;

typedef enum {
	SLEEPING,
	FINDING,
	FOUND,
}STATUS_T;

// Node Statistics
typedef struct {
	int round;
	int numExploreMessages;
	int numAcks;
	int numNacks;
}Statistics;

typedef struct {
	int front;
	int back;
	int nodeId;
	pthread_mutex_t queueMutex;
	message_t elements[MAX_QUEUE_SIZE];
}queue_t;

// Node data
typedef struct {
	int nodeId;
	int uId;
	int parentId;
	int leaderId;
	int level;
	int componentId;
	int roundDone;
	int connected;
	int waitListCount;
	int MWOEResponses;
	int recvBufferSize;
	int algorithmEnd;

	STATUS_T status;

	pthread_mutex_t threadMutex;
	pthread_mutex_t recvSizeMutex;
	pthread_cond_t roundFinishCondition;
	pthread_t threadId;

	MWOEData myMWOEData;

	int *connectivity;
	int *spanningTreeConnectivity; // classifies edge as tree edge, rejected edge or basic edge

	queue_t *recvQueue; // queue of received messages
	queue_t *processQueue; // queue of messages to be processed
	queue_t *deferQueue; // queue of deferred messages

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

message_t *getNextElement(queue_t *q);
message_t *peekNextElement(queue_t *q);

void addElementToQueue(queue_t *q, message_t msg);


#endif
