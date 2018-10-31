#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include "algorithm.h"
#include "simulator.h"
#include "log.h"

void waitForNodesToFinish() {
	int index = 0;
	for (index = 0; index < globalState.nProcess; index++) {
		pthread_join(globalState.nodeStates[index]->threadId, NULL);
	}
}

void notifyNodes(MASTER_NOTIFICATION notification) {
	globalState.masterNotification = notification;
	while (globalState.waitCount != globalState.nProcess) {
		usleep(THREAD_SLEEP_MICRO);
	}
	pthread_cond_broadcast(&globalState.synchronyCondition);
}

void * masterRoutine(void *masterArgs) {
	int nodeId = 0;
	int done = 0;

	globalState.currentRound = 0;

	while (!done) {
		globalState.currentRound += 1;
		INFO("Master: START ROUND %d\n", globalState.currentRound);
		notifyNodes(START_ROUND);

		for (nodeId = 0; nodeId < globalState.nProcess; nodeId++) {
			DEBUG("Master: Node[%d] status %d\n", nodeId, globalState.nodeStates[nodeId]->roundDone);

			pthread_mutex_lock(&globalState.nodeStates[nodeId]->threadMutex);
			while (!globalState.nodeStates[nodeId]->roundDone) {
				DEBUG("Master: waiting on Node[%d]\n", nodeId);
				pthread_cond_wait(&globalState.nodeStates[nodeId]->roundFinishCondition, &globalState.nodeStates[nodeId]->threadMutex);
			}
			globalState.nodeStates[nodeId]->roundDone = 0;
			pthread_mutex_unlock(&globalState.nodeStates[nodeId]->threadMutex);

			DEBUG("Master: Node[%d] finished ROUND %d\n", nodeId, globalState.currentRound);
		}
		if (algorithmEnd() == 1) {
			done = 1;
		}
		INFO("Master: END ROUND %d\n", globalState.currentRound);
	}

	printStatistics();
	notifyNodes(END);
	waitForNodesToFinish();
	DEBUG("Master THREAD done\n");
	return NULL;
}

void notifyMaster(int nodeId) {
	// reply to master thread ??
	DEBUG("Node[%d]: Signalling master\n", nodeId);
	pthread_cond_signal(&globalState.nodeStates[nodeId]->roundFinishCondition);
}

int processRoundK(int roundK, int nodeId) {
	pthread_mutex_lock(&globalState.nodeStates[nodeId]->threadMutex);
	DEBUG("NODE[%d]: ROUND %d\n", nodeId, roundK);

	// generate messages
	int numMessages = generateMessages(nodeId);
	DEBUG("Node[%d]: Generate messages(%d) done\n", nodeId, numMessages);

	// while recvBufferSize != connected size, keep receiving messages
	while (globalState.nodeStates[nodeId]->recvBufferSize < globalState.nProcess) {
		// wait for messages from all connected nodes
		DEBUG("Node[%d]: Waiting. Received: %d\n", nodeId, globalState.nodeStates[nodeId]->recvBufferSize);
	}
	// consume received messages
	consumeMessages(nodeId);

	globalState.nodeStates[nodeId]->roundDone = 1;
	pthread_mutex_unlock(&globalState.nodeStates[nodeId]->threadMutex);
	return 0;
}

void waitForMaster() {
	pthread_mutex_lock(&globalState.masterMutex);
	globalState.waitCount++;
	pthread_cond_wait(&globalState.synchronyCondition, &globalState.masterMutex);
	globalState.waitCount--;
	pthread_mutex_unlock(&globalState.masterMutex);
}

void * nodeRoutine(void *nodeArgs) {
	// wait for START_ROUND(k) message from master thread
	int nodeId = *((int*)nodeArgs);
	int done = 0;
	while (!done) {
		DEBUG("Node[%d]: waiting on master\n", nodeId);

		waitForMaster();

		if (globalState.masterNotification == END) {
			DEBUG("Node[%d]: FINISH\n", nodeId);
			break;
		}

		processRoundK(globalState.currentRound, nodeId);

		notifyMaster(nodeId);

		DEBUG("Node[%d]: Round %d done\n", nodeId, globalState.currentRound);
	}
	DEBUG("Node[%d] DONE\n", nodeId);
	return NULL;
}

void simulate() {

	// update global state with master thread
	int nProcess = globalState.nProcess;
	int index = 0;

	pthread_mutex_init(&globalState.masterMutex, NULL);
	pthread_cond_init(&globalState.synchronyCondition, NULL);

	for (index = 0; index < nProcess; index++) {
		// create threads to simulate nodes
		pthread_mutex_init(&(globalState.nodeStates[index]->threadMutex), NULL);
		pthread_cond_init(&globalState.nodeStates[index]->roundFinishCondition, NULL);
		pthread_create(&globalState.nodeStates[index]->threadId, NULL, nodeRoutine, &globalState.nodeStates[index]->nodeId);
		// update global state with thread info
	}

	// create master thread
	pthread_create(&globalState.masterThread, NULL, masterRoutine, NULL);
	pthread_join(globalState.masterThread, NULL);
}

void finish() {
	int nodeId = 0;
	for (nodeId = 0; nodeId < globalState.nProcess; nodeId++) {
		free(globalState.nodeStates[nodeId]->connectivity);
		free(globalState.nodeStates[nodeId]->spanningTreeConnectivity);
		pthread_mutex_destroy(&(globalState.nodeStates[nodeId]->recvQueue->queueMutex));
		free(globalState.nodeStates[nodeId]->recvQueue);
		pthread_mutex_destroy(&(globalState.nodeStates[nodeId]->processQueue->queueMutex));
		free(globalState.nodeStates[nodeId]->processQueue);
		free(globalState.nodeStates[nodeId]);

		pthread_mutex_destroy(&globalState.nodeStates[nodeId]->threadMutex);
		pthread_cond_destroy(&globalState.nodeStates[nodeId]->roundFinishCondition);
	}
	pthread_cond_destroy(&globalState.synchronyCondition);
	pthread_mutex_destroy(&globalState.masterMutex);
	free(globalState.nodeStates);
}

void printArgs(int n, int *uniqueIds, int connectivity[][n]) {
	INFO("Number of Nodes: %d\n", n);

	int index = 0;
	int connectivityIndex = 0;

	for (index = 0; index < n; index++) {
		INFO("Node[%d]: UniqueId[%d]\n", index, uniqueIds[index]);
		for (connectivityIndex = 0; connectivityIndex < n; connectivityIndex ++) {
			INFO("\t Connectivity(%d): %d\n", index, connectivity[index][connectivityIndex]);
		}
	}
}

void printNode(int nodeId) {
	INFO("Node[%d]: connectivity: %d\n", nodeId, globalState.nodeStates[nodeId]->connected);
	int index = 0;
	for (index = 0; index < globalState.nProcess; index++) {
		INFO("\t %d connectivity: %d\n", index, globalState.nodeStates[nodeId]->connectivity[index]);
	}
}

message_t *peekNextElement(queue_t *q) {
	pthread_mutex_lock(&(q->queueMutex));
	message_t *msg = NULL;
	if (q->front == q->back) {
		msg = NULL;
	} else {
		msg = &(q->elements[q->back]);
	}
	pthread_mutex_unlock(&(q->queueMutex));
	return msg;
}

message_t *getNextElement(queue_t *q) {
	pthread_mutex_lock(&(q->queueMutex));
	message_t *msg = NULL;
	if (q->front == q->back) {
		msg = NULL;
	} else {
		int id = q->back;
		q->back = (q->back + 1)%MAX_QUEUE_SIZE;
		msg = &(q->elements[id]);
	}
	pthread_mutex_unlock(&(q->queueMutex));
	return msg;
}

void addElementToQueue(queue_t *q, message_t msg) {
	pthread_mutex_lock(&(q->queueMutex));
	if ((q->front + 1)%MAX_QUEUE_SIZE == q->back) {
		INFO("Max recv Queue size from %d %d %d reached\n", q->nodeId, q->front, q->back);
	} else {
		memcpy(&(q->elements[q->front]), &msg, sizeof(message_t));
		q->front = (q->front + 1)%MAX_QUEUE_SIZE;
	}
	pthread_mutex_unlock(&(q->queueMutex));
}

int main(int argc, char *argv[]) {

	int index = 0;
	int n = 0;
	int connectivityIndex = 0;

	scanf("%d", &n);

	int uniqueIds[n];
	int connectivity[n][n];

	for (index = 0; index < n; index++) {
		scanf("%d", &uniqueIds[index]);
	}

	for (index = 0; index < n; index++) {
		int edgeLink = 0;
		for (edgeLink = 0; edgeLink < n; edgeLink++) {
			scanf("%d", &connectivity[index][edgeLink]);
		}
	}
	
	// printArgs(n, uniqueIds, connectivity);
	
	globalState.nProcess = n;
	globalState.nodeStates = (NodeState **)malloc(sizeof(NodeState *)*n);
	if (globalState.nodeStates == NULL) {
		DEBUG("Failed to allocate memory\n");
		return 0;
	}
	for (index = 0; index < globalState.nProcess; index++) {
		NodeState *nodeState = (NodeState *)malloc(sizeof(NodeState));

		if (nodeState) {
			memset(nodeState, 0, sizeof(NodeState));

			DEBUG("Node: %d initing\n", index);
			nodeState->nodeId = index;
			nodeState->uId = uniqueIds[index];

			nodeState->connected = 0;
			nodeState->parentId = index;
			nodeState->leaderId = index;
			nodeState->level = 0;
			nodeState->componentId = index;
			nodeState->MWOEResponses = 0;
			nodeState->recvBufferSize = 0;
			nodeState->status = 0;

			memset(&(nodeState->myMWOEData), 0, sizeof(MWOEData));

			nodeState->connectivity = (int *)malloc(n*sizeof(int));
			memset(nodeState->connectivity, 0, n);
			// TODO check malloc failure:

			nodeState->spanningTreeConnectivity = (int *)malloc(n*sizeof(int));
			memset(nodeState->spanningTreeConnectivity, 0, n);

			// children list init
			nodeState->children = (int *)malloc(n*sizeof(int));
			memset(nodeState->children, 0, n);

			nodeState->recvQueue = (queue_t *)malloc(n * sizeof(queue_t));
			pthread_mutex_init(&nodeState->recvQueue->queueMutex, NULL);
			nodeState->processQueue = (queue_t *)malloc(n * sizeof(queue_t));
			pthread_mutex_init(&nodeState->processQueue->queueMutex, NULL);

			for (connectivityIndex = 0; connectivityIndex < n; connectivityIndex ++) {
				nodeState->connectivity[connectivityIndex] = connectivity[index][connectivityIndex];

				if (connectivityIndex == index) {
					nodeState->connectivity[connectivityIndex] = 1;
				}

				if (nodeState->connectivity[connectivityIndex] != 0) {
					nodeState->connected+=1;
					if (connectivityIndex != index) {
						nodeState->recvQueue[connectivityIndex].front = 0;
						nodeState->recvQueue[connectivityIndex].back = 0;
						nodeState->recvQueue[connectivityIndex].nodeId = connectivityIndex;
						nodeState->processQueue[connectivityIndex].front = 0;
						nodeState->processQueue[connectivityIndex].back = 0;
						nodeState->processQueue[connectivityIndex].nodeId = connectivityIndex;
					}
				}
			}
			DEBUG("Node[%d]: connected: %d\n", index, nodeState->connected);

			memset(&nodeState->myMWOEData, 0, sizeof(MWOEData));
			globalState.nodeStates[index] = nodeState;
			globalState.nodeStates[index]->roundDone = 0;

			DEBUG("Node[%d]: Init Done\n", index);
			// printNode(index);
		} else {
			DEBUG("Node %d init failed\n", index);
			// TODO clear all mallocs
			return 0;
		}
	}
	simulate();
	finish();
	return 0;
}
