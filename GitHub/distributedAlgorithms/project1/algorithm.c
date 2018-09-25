#include <pthread.h>
#include <string.h>
#include "algorithm.h"
#include "simulator.h"
#include "messages.h"
#include "log.h"

void printStatistics() {
	int nodeIndex = 0;
	for (nodeIndex = 0; nodeIndex < globalState.nProcess; nodeIndex++) {
		DEBUG("Node[%d]: (Parent %d Leader %d WaitListCount: %d Rounds: %d Explore: %d Acks: %d Nacks: %d)\n",
				nodeIndex,
				(globalState.nodeStates[nodeIndex]->parentId),
				(globalState.nodeStates[nodeIndex]->leaderId),
				(globalState.nodeStates[nodeIndex]->waitListCount),
				(globalState.nodeStates[nodeIndex]->stats).round,
				(globalState.nodeStates[nodeIndex]->stats).numExploreMessages,
				(globalState.nodeStates[nodeIndex]->stats).numAcks,
				(globalState.nodeStates[nodeIndex]->stats).numNacks);
	}
}

void generateMessages(message_t *messages, int nodeId) {
	// messages is the sendMsg
	// check received messages
	// 	explore:
	//		remove msg.from from waitList
	//		node.leaderId > msg.value ?
	//			node.leaderId = msg.value
	//			add explore(node.nodeId, node.leaderId) to all other nodes
	//		else if sendMsg[msg.from] != explore:
	//				add nack to msg.from
	//	nack or ack:
	//		remove msg.from from waitList

	int index = 0;
	int childIndex = 0;

	pthread_mutex_lock(&globalState.nodeStates[nodeId]->recvBufferMutex);
	(globalState.nodeStates[nodeId]->stats).round ++;
	if (globalState.currentRound == 1) {
		globalState.nodeStates[nodeId]->leaderId = globalState.nodeStates[nodeId]->uId;
		globalState.nodeStates[nodeId]->parentId = nodeId;
		for (index = 0; index < globalState.nProcess; index++) {
			if (nodeId != index &&
					globalState.nodeStates[nodeId]->connectivity[index] == 1) {
				DEBUG("Node[%d]: Send Explore to %d\n", nodeId, index);
				message_t sendMsg = {
					.type = MESSAGE_TYPE_EXPLORE,
					.toId = index,
					.fromId = nodeId,
					.value = globalState.nodeStates[nodeId]->uId,
				};
				memcpy(&messages[index], &sendMsg, sizeof(message_t));
			}
		}
		pthread_mutex_unlock(&globalState.nodeStates[nodeId]->recvBufferMutex);
		return;
	}

	for (index = 0; index < globalState.nodeStates[nodeId]->connected; index++) {
		message_t msg = globalState.nodeStates[nodeId]->processBuffer[index];
		logMessage(nodeId, msg);
		if (msg.type == MESSAGE_TYPE_EXPLORE || msg.type == MESSAGE_TYPE_NACK_EXPLORE) {
			message_t sendMsg = {
				.type = MESSAGE_TYPE_NONE,
				.fromId = nodeId,
				.value = globalState.nodeStates[nodeId]->uId,
				.toId = nodeId,
			};
			if (msg.type == MESSAGE_TYPE_NACK_EXPLORE) {
				globalState.nodeStates[nodeId]->waitListCount--;
				globalState.nodeStates[nodeId]->waitList[msg.fromId] = 0;
				DEBUG("Node[%d]: WaitListCount: %d\n", nodeId, globalState.nodeStates[nodeId]->waitListCount);
			}
			if (globalState.nodeStates[nodeId]->leaderId > msg.value) {
				globalState.nodeStates[nodeId]->leaderId = msg.value;
				globalState.nodeStates[nodeId]->parentId = msg.fromId;
				// send explore msg with parent nodeId and leader leaderId to all connected nodes
				DEBUG("Node[%d]: Send Explore to all nodes except %d\n", nodeId, msg.fromId);
				sendMsg.value = globalState.nodeStates[nodeId]->leaderId;
				for (childIndex = 0; childIndex < globalState.nProcess; childIndex++) {
					if (globalState.nodeStates[nodeId]->connectivity[childIndex] == 1 && childIndex != nodeId) {
						sendMsg.toId = childIndex;
						if (msg.fromId == childIndex) {
							DEBUG("Node[%d]: Send ACK to %d\n", nodeId, sendMsg.toId);
							sendMsg.type = MESSAGE_TYPE_ACK;
							(globalState.nodeStates[nodeId]->stats).numAcks ++;
						} else {
							sendMsg.type = MESSAGE_TYPE_EXPLORE;
							if (messages[childIndex].type == MESSAGE_TYPE_ACK ||
									messages[childIndex].type == MESSAGE_TYPE_NACK ||
									messages[childIndex].type == MESSAGE_TYPE_NACK_EXPLORE) {
								sendMsg.type = MESSAGE_TYPE_NACK_EXPLORE;
								DEBUG("Node[%d]: Send NACK_EXPLORE to %d\n", nodeId, sendMsg.toId);
							} else {
								DEBUG("Node[%d]: Send EXPLORE to %d\n", nodeId, sendMsg.toId);
							}
							(globalState.nodeStates[nodeId]->stats).numExploreMessages ++;
						}
						memcpy(&messages[childIndex], &sendMsg, sizeof(message_t));
						logMessage(nodeId, messages[childIndex]);
					}
				}
			} else if (globalState.nodeStates[nodeId]->leaderId == msg.value) {
				// send nack to this node since leader is same for both nodes
				DEBUG("Node[%d]: Send NACK to %d\n", nodeId, msg.fromId);
				sendMsg.value = globalState.nodeStates[nodeId]->leaderId;
                                sendMsg.type = MESSAGE_TYPE_NACK;
                                sendMsg.toId = msg.fromId;
                                memcpy(&messages[msg.fromId], &sendMsg, sizeof(message_t));
				(globalState.nodeStates[nodeId]->stats).numNacks ++;
			} else {
				// send nack to this node
				logMessage(nodeId, messages[msg.fromId]);
                       	       	sendMsg.type = MESSAGE_TYPE_NACK;
				if (messages[msg.fromId].type == MESSAGE_TYPE_EXPLORE) {
					sendMsg.type = MESSAGE_TYPE_NACK_EXPLORE;
					(globalState.nodeStates[nodeId]->stats).numExploreMessages ++;
					DEBUG("Node[%d]: Sending NACK_EXPLORE to %d\n", nodeId, msg.fromId);
				} else {
					DEBUG("Node[%d]: Sending NACK to %d\n", nodeId, msg.fromId);
				}
				sendMsg.value = globalState.nodeStates[nodeId]->leaderId;
                       	       	sendMsg.toId = msg.fromId;
                       	       	memcpy(&messages[msg.fromId], &sendMsg, sizeof(message_t));
				(globalState.nodeStates[nodeId]->stats).numNacks ++;
			}
		} else if (msg.type == MESSAGE_TYPE_ACK ||
				msg.type == MESSAGE_TYPE_NACK) {
			// remove from waitlist
			globalState.nodeStates[nodeId]->waitListCount--;
			DEBUG("Node[%d]: WaitListCount: %d\n", nodeId, globalState.nodeStates[nodeId]->waitListCount);
			globalState.nodeStates[nodeId]->waitList[msg.fromId] = 0;
		}
	}
	pthread_mutex_unlock(&globalState.nodeStates[nodeId]->recvBufferMutex);
}

void consumeMessages(int nodeId) {
	int index = 0;
	pthread_mutex_lock(&globalState.nodeStates[nodeId]->recvBufferMutex);

	for (index = 0; index < globalState.nodeStates[nodeId]->recvBufferSize; index++) {
		logMessage(nodeId, globalState.nodeStates[nodeId]->recvBuffer[index]);
		memcpy(&globalState.nodeStates[nodeId]->processBuffer[index], &globalState.nodeStates[nodeId]->recvBuffer[index], sizeof(message_t));
	}

	globalState.nodeStates[nodeId]->processBufferSize = globalState.nodeStates[nodeId]->recvBufferSize;
	globalState.nodeStates[nodeId]->recvBufferSize = 0;

	pthread_mutex_unlock(&globalState.nodeStates[nodeId]->recvBufferMutex);
}
