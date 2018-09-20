#include <pthread.h>
#include <string.h>
#include "algorithm.h"
#include "simulator.h"
#include "messages.h"
#include "log.h"

void generateMessages(message_t *messages, int nodeId) {
	int index = 0;
	int messageIndex = 0;

	DEBUG("Node[%d]: Generate Messages\n", nodeId);
	DEBUG("Node[%d]: globalState.nProcess: %d\n", nodeId, globalState.nProcess);
	for (index = 0; index < globalState.nProcess; index++) {
		DEBUG("Node[%d]: connected to %d ? %d\n", nodeId, index, globalState.nodeStates[nodeId]->connectivity[index]);
		if (globalState.nodeStates[nodeId]->connectivity[index] == 1 && nodeId != index) {
			DEBUG("Node[%d]: generating message to %d\n", nodeId, index);
			message_t message = {
				.type = MESSAGE_TYPE_NONE,
				.value = globalState.nodeStates[nodeId]->uId,
				.fromId = nodeId,
				.toId = index,
			};
			memcpy(&messages[messageIndex++], &message, sizeof(message));
			logMessage(nodeId, messages[messageIndex-1]);
		}
	}
}

void consumeMessages(int nodeId) {
	int index = 0;
	pthread_mutex_lock(&globalState.nodeStates[nodeId]->recvBufferMutex);

	for (index = 0; index < globalState.nodeStates[nodeId]->recvBufferSize; index++) {
		logMessage(nodeId, globalState.nodeStates[nodeId]->recvBuffer[index]);
	}

	globalState.nodeStates[nodeId]->recvBufferSize = 0;

	pthread_mutex_unlock(&globalState.nodeStates[nodeId]->recvBufferMutex);
}
