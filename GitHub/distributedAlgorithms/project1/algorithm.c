#include <pthread.h>
#include <string.h>
#include "algorithm.h"
#include "simulator.h"
#include "messages.h"
#include "log.h"


int algorithmEnd() {
	int nodeIndex = 0;
	int finalLeader = globalState.nodeStates[nodeIndex]->leaderId;
	for (nodeIndex = 0; nodeIndex < globalState.nProcess; nodeIndex++) {
		if (globalState.nodeStates[nodeIndex]->leaderId != finalLeader) {
			return 0;
		}
	}
	return 1;
}

void generateMessages(message_t *messages, int nodeId) {
	int index = 0;
	int messageIndex = 0;
	int i = 0;
	pthread_mutex_lock(&globalState.nodeStates[nodeId]->recvBufferMutex);
	DEBUG("Node[%d]: Generate Messages\n", nodeId);
	DEBUG("Node[%d]: globalState.nProcess: %d\n", nodeId, globalState.nProcess);
	if(globalState.currentRound == 1){
		for (index = 0; index < globalState.nProcess; index++) {
			DEBUG("Node[%d]: connected to %d ? %d\n", nodeId, index, globalState.nodeStates[nodeId]->connectivity[index]);
			if (globalState.nodeStates[nodeId]->connectivity[index] == 1 && nodeId != index) {
				DEBUG("Node[%d]: generating message to %d\n", nodeId, index);
				message_t message = {
					.type = MESSAGE_TYPE_EXPLORE,
					.value = globalState.nodeStates[nodeId]->uId,
					.fromId = nodeId,
					.toId = index,
				};
				memcpy(&messages[messageIndex++], &message, sizeof(message));
				logMessage(nodeId, messages[messageIndex-1]);
			}
		}
	}else{
		if( globalState.nodeStates[nodeId]->parentUpdated == 1){
			for (i = 0; i < globalState.nProcess; i++) {
				message_t msg = {
					.type = MESSAGE_TYPE_EXPLORE,
					.toId = index,
					.fromId = nodeId,
					.value = globalState.nodeStates[nodeId]->uId,
				};
				if (globalState.nodeStates[nodeId]->connectivity[i] == 1 && i != nodeId) {
					msg.toId = i;
					if (globalState.nodeStates[nodeId]->parentId == i) {
						DEBUG("Node[%d]: Send ACK to %d\n", nodeId, msg.toId);
						msg.type = MESSAGE_TYPE_ACK;
					} else {
						if (messages[i].type == MESSAGE_TYPE_ACK || messages[i].type == MESSAGE_TYPE_NACK) {
							msg.type = MESSAGE_TYPE_NACK;
							DEBUG("Node[%d]: Send NACK to %d\n", nodeId, msg.toId);
						} else {
							DEBUG("Node[%d]: Send EXPLORE to %d\n", nodeId, msg.toId);
						}
					}
					memcpy(&messages[i], &msg, sizeof(message_t));
				}
			}
		}
	}
	pthread_mutex_unlock(&globalState.nodeStates[nodeId]->recvBufferMutex);
}

void consumeMessages(message_t *messages, int nodeId) {
	int index = 0;
	pthread_mutex_lock(&globalState.nodeStates[nodeId]->recvBufferMutex);
	for (index = 0; index < globalState.nodeStates[nodeId]->recvBufferSize; index++) {
		logMessage(nodeId, globalState.nodeStates[nodeId]->recvBuffer[index]);
		message_t msg = globalState.nodeStates[nodeId]->recvBuffer[index];
		if(msg.type == MESSAGE_TYPE_EXPLORE){
			DEBUG("Explore Message details: From: %d, To: %d, Value: %d\n", msg.fromId, msg.toId, msg.value);
			if (msg.value > globalState.nodeStates[nodeId]->leaderId) {
				globalState.nodeStates[nodeId]->leaderId = msg.value;
				globalState.nodeStates[nodeId]->parentId = msg.fromId;
				globalState.nodeStates[nodeId]->parentUpdated = 1;
			}else if(msg.value == globalState.nodeStates[nodeId]-> leaderId) {
				DEBUG("Node[%d]: Send NACK to %d\n", nodeId, msg.fromId);
			}else {
				message_t reply = {
					.type = MESSAGE_TYPE_EXPLORE,
					.toId = index,
					.fromId = nodeId,
					.value = globalState.nodeStates[nodeId]->uId,
				};
				reply.type = MESSAGE_TYPE_NACK;
				if (messages[msg.fromId].type == MESSAGE_TYPE_EXPLORE) {
					reply.type = MESSAGE_TYPE_NACK;
					DEBUG("Node[%d]: Sending NACK_EXPLORE to %d\n", nodeId, msg.fromId);
				} else {
					DEBUG("Node[%d]: Sending NACK to %d\n", nodeId, msg.fromId);
				}
				reply.value = globalState.nodeStates[nodeId]->leaderId;
				reply.toId = msg.fromId;
				memcpy(&messages[msg.fromId], &reply, sizeof(message_t));
				
			}
		}else if (msg.type == MESSAGE_TYPE_ACK) {
			globalState.nodeStates[nodeId]->children[msg.fromId] = 1;
		} else {
			globalState.nodeStates[nodeId]->children[msg.fromId] = 0;
		}
	}
	globalState.nodeStates[nodeId]->recvBufferSize = 0;

	pthread_mutex_unlock(&globalState.nodeStates[nodeId]->recvBufferMutex);
}
