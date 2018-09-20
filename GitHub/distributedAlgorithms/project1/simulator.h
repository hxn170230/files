#ifndef SIMULATOR_H

#define THREAD_SLEEP_MICRO 10000

typedef struct {
	int round;
	int numExploreMessages;
	int numAcks;
	int numNacks;
	int parentChange;
}Statistics;

typedef struct {
	int nodeId;
	int uId;
	int parentId;
	int roundDone;
	int *connectivity;

	pthread_t threadId;
	pthread_mutex_t threadMutex;
	pthread_cond_t roundFinishCondition;

	Statistics *stats;
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

static GlobalState globalState;

#endif
