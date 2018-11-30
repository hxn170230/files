#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include "messages.h"
#include "algorithm.h"
#include "simulator.h"
#include "log.h"

void sendMessage(int fromId, int toNodeId, message_t message) {
	pthread_mutex_lock(&globalState.nodeStates[toNodeId]->recvBufferMutex);

	logMessage(fromId, message);
	memcpy(&globalState.nodeStates[toNodeId]->recvBuffer[globalState.nodeStates[toNodeId]->recvBufferSize++], &message, sizeof(message));

	pthread_mutex_unlock(&globalState.nodeStates[toNodeId]->recvBufferMutex);
}

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

#define MAX_ROUNDS 1000
void * masterRoutine(void *masterArgs) {
	int nodeId = 0;
	int done = 0;

	globalState.currentRound = 0;

	while(!done){
		globalState.currentRound += 1;
		DEBUG("Master: Triggering ROUND %d\n", globalState.currentRound);
		notifyNodes(START_ROUND);

		for (nodeId = 0; nodeId < globalState.nProcess; nodeId++) {
			DEBUG("Master: Node[%d] status %d\n", nodeId, globalState.nodeStates[nodeId]->roundDone);

			pthread_mutex_lock(&globalState.nodeStates[nodeId]->threadMutex);
			while (!globalState.nodeStates[nodeId]->roundDone) {
				DEBUG("Master: waiting on Node[%d]\n", nodeId);
				pthread_cond_wait(&globalState.nodeStates[nodeId]->roundFinishCondition, &globalState.nodeStates[nodeId]->threadMutex);
			}
			globalState.nodeStates[nodeId]->roundDone = 0;
			globalState.nodeStates[nodeId]->parentId = nodeId;
			globalState.nodeStates[nodeId]->leaderId = nodeId;
			pthread_mutex_unlock(&globalState.nodeStates[nodeId]->threadMutex);

			DEBUG("Master: Node[%d] finished ROUND %d\n", nodeId, globalState.currentRound);
		}
		if (algorithmEnd() == 1) {
			done = 1;
		}
		DEBUG("Master: END ROUND %d\n", globalState.currentRound);
	}

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
	int messageIndex = 0;
	message_t messages[globalState.nodeStates[nodeId]->connected];
	memset(messages, 0, sizeof(messages));

	pthread_mutex_lock(&globalState.nodeStates[nodeId]->threadMutex);
	DEBUG("NODE[%d]: ROUND %d\n", nodeId, roundK);

	// generate messages
	generateMessages(messages, nodeId);
	DEBUG("Node[%d]: Generate messages done\n", nodeId);

	// send messages
	for (messageIndex = 0; messageIndex < globalState.nodeStates[nodeId]->connected; messageIndex++) {
		sendMessage(nodeId, messages[messageIndex].toId, messages[messageIndex]);
	}

	// while recvBufferSize != connected size, keep receiving messages
	while (globalState.nodeStates[nodeId]->recvBufferSize != globalState.nodeStates[nodeId]->connected) {
		// wait for messages from all connected nodes
		DEBUG("Node[%d]: recv buffer size: %d\n", nodeId, globalState.nodeStates[nodeId]->recvBufferSize);
	}
	// consume received messages
	consumeMessages(messages, nodeId);

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

	DEBUG("Master mutex init\n");
	pthread_mutex_init(&globalState.masterMutex, NULL);
	DEBUG("Master condition init\n");
	pthread_cond_init(&globalState.synchronyCondition, NULL);

	for (index = 0; index < nProcess; index++) {
		// create threads to simulate nodes
		pthread_mutex_init(&globalState.nodeStates[index]->recvBufferMutex, NULL);
		DEBUG("Node[%d] mutex init\n", index);
		pthread_mutex_init(&(globalState.nodeStates[index]->threadMutex), NULL);
		DEBUG("Node[%d] condition init\n", index);
		pthread_cond_init(&globalState.nodeStates[index]->roundFinishCondition, NULL);
		DEBUG("Node[%d] thread init\n", index);
		pthread_create(&globalState.nodeStates[index]->threadId, NULL, nodeRoutine, &globalState.nodeStates[index]->nodeId);
		// update global state with thread info
	}

	// create master thread
	DEBUG("Master thread init\n");
	pthread_create(&globalState.masterThread, NULL, masterRoutine, NULL);
	pthread_join(globalState.masterThread, NULL);
}

void finish() {
	int nodeId = 0;
	for (nodeId = 0; nodeId < globalState.nProcess; nodeId++) {
		free(globalState.nodeStates[nodeId]->connectivity);
		free(globalState.nodeStates[nodeId]->stats);
		free(globalState.nodeStates[nodeId]->recvBuffer);
		free(globalState.nodeStates[nodeId]);

		pthread_mutex_destroy(&globalState.nodeStates[nodeId]->threadMutex);
		pthread_mutex_destroy(&globalState.nodeStates[nodeId]->recvBufferMutex);
		pthread_cond_destroy(&globalState.nodeStates[nodeId]->roundFinishCondition);
	}
	pthread_cond_destroy(&globalState.synchronyCondition);
	pthread_mutex_destroy(&globalState.masterMutex);
	free(globalState.nodeStates);
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
	
	globalState.nProcess = n;
	globalState.nodeStates = (NodeState **)malloc(sizeof(NodeState *)*n);
	if (globalState.nodeStates == NULL) {
		DEBUG("Failed to allocate memory\n");
		return 0;
	}
	for (index = 0; index < globalState.nProcess; index++) {
		NodeState *nodeState = (NodeState *)malloc(sizeof(NodeState));
		if (nodeState) {
			DEBUG("Node: %d initing\n", index);
			nodeState->nodeId = index;
			nodeState->uId = uniqueIds[index];
			nodeState->parentId = -1;
			nodeState->connectivity = (int *)malloc(n*sizeof(int));
			// TODO check malloc failure:
			nodeState->connected = 0;
			for (connectivityIndex = 0; connectivityIndex < n; connectivityIndex ++) {
				nodeState->connectivity[connectivityIndex] = connectivity[index][connectivityIndex];
				if (connectivity[index][connectivityIndex] != 0 && index != connectivityIndex) {
					nodeState->connected+=1;
				}
			}
			DEBUG("Node[%d]: connected: %d\n", index, nodeState->connected);
			nodeState->stats = (Statistics *)malloc(sizeof(Statistics));
			// TODO check malloc failure
			globalState.nodeStates[index] = nodeState;
			globalState.nodeStates[index]->roundDone = 0;

			// recv buffer init
			DEBUG("Node[%d]: recvBuffer init\n", index);
			globalState.nodeStates[index]->recvBuffer = (message_t *)malloc(sizeof(message_t)*globalState.nodeStates[index]->connected);
			DEBUG("Node[%d]: recvBuffer init done\n", index);
			globalState.nodeStates[index]->recvBufferSize = 0;
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
