#include <pthread.h>
#include <string.h>
#include "algorithm.h"
#include "simulator.h"
#include "messages.h"
#include "log.h"

void printAdjMatrix(int n, int adjMatrix[][n]) {
	int indexi = 0;
	int indexj = 0;

	for (indexi = 0; indexi < n; indexi++) {
		for (indexj = 0; indexj < n; indexj++) {
			INFO("%d ", adjMatrix[indexi][indexj]);
		}
		INFO("\n");
	}
}

int getNumMessages() {
	int index = 0;
	int total = 0;
	for (index = 0; index < globalState.nProcess; index++) {
		total += (globalState.nodeStates[index]->stats).numExploreMessages;
		total += (globalState.nodeStates[index]->stats).numAcks;
		total += (globalState.nodeStates[index]->stats).numNacks;
	}
	return total;
}

void printStatistics() {
	int nodeIndex = 0;
	int childIndex = 0;
	int leaderId = 0;

	int adjMatrix[globalState.nProcess][globalState.nProcess];

	for (nodeIndex = 0; nodeIndex < globalState.nProcess; nodeIndex++) {
		for (childIndex = 0; childIndex < globalState.nProcess; childIndex++) {
			if (nodeIndex == childIndex) {
				adjMatrix[nodeIndex][nodeIndex] = 1;
			} else {
				adjMatrix[nodeIndex][childIndex] = 0;
			}
		}
	}

	for (nodeIndex = 0; nodeIndex < globalState.nProcess; nodeIndex++) {

		leaderId = globalState.nodeStates[nodeIndex]->leaderId;
		INFO("Node[%d]: Parent: %d Leader: %d\n",
				nodeIndex,
				(globalState.nodeStates[nodeIndex]->parentId),
				(globalState.nodeStates[nodeIndex]->leaderId));

		adjMatrix[nodeIndex][globalState.nodeStates[nodeIndex]->parentId] = 1;
		adjMatrix[globalState.nodeStates[nodeIndex]->parentId][nodeIndex] = 1;

		INFO("\t Children: ( ");
		for (childIndex = 0; childIndex < globalState.nProcess; childIndex++) {
			if (globalState.nodeStates[nodeIndex]->children[childIndex] == 1) {
				INFO("%d ", childIndex);
				adjMatrix[nodeIndex][childIndex] = 1;
				adjMatrix[childIndex][nodeIndex] = 1;
			}
		}
		INFO(")\n\n");

		INFO("\t Stats:");
		INFO("\t (WaitListCount: %d Rounds: %d Explore: %d Acks: %d Nacks: %d)\n\n",
				(globalState.nodeStates[nodeIndex]->waitListCount),
				(globalState.nodeStates[nodeIndex]->stats).round,
				(globalState.nodeStates[nodeIndex]->stats).numExploreMessages,
				(globalState.nodeStates[nodeIndex]->stats).numAcks,
				(globalState.nodeStates[nodeIndex]->stats).numNacks);

	}

	INFO("####################################################################################\n");
	INFO("Total Number of Rounds : \t %d\n", globalState.currentRound);
	INFO("Total Number of Messages: \t %d\n", getNumMessages());
	INFO("LEADER : \t\t\t %d\n", leaderId);
	INFO("Tree Adj Matrix: \n");
	printAdjMatrix(globalState.nProcess, adjMatrix);
	INFO("####################################################################################\n");
}

int algorithmEnd() {
	int nodeIndex = 0;
	for (nodeIndex = 0; nodeIndex < globalState.nProcess; nodeIndex++) {
		if (globalState.nodeStates[nodeIndex]->waitListCount != 0) {
			return 0;
		}
	}
	return 1;
}

void generateMessages(message_t *messages, int nodeId) {

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
				(globalState.nodeStates[nodeId]->stats).numExploreMessages ++;
			}
		}
		pthread_mutex_unlock(&globalState.nodeStates[nodeId]->recvBufferMutex);
		return;
	}

	// check received messages
	for (index = 0; index < globalState.nodeStates[nodeId]->connected; index++) {
		message_t msg = globalState.nodeStates[nodeId]->processBuffer[index];
		logMessage(nodeId, msg);
		// explore:
		// 	remove msg.from from waitList
		//	node.leaderId > msg.value ?
		//	node.leaderId = msg.value
		//		add explore(node.nodeId, node.leaderId) to all other nodes
		//	else if sendMsg[msg.from] != explore:
		//	add nack to msg.from
		// nack or ack:
		//		remove msg.from from waitList
		if (msg.type == MESSAGE_TYPE_EXPLORE || msg.type == MESSAGE_TYPE_NACK_EXPLORE) {
			message_t sendMsg = {
				.type = MESSAGE_TYPE_NONE,
				.fromId = nodeId,
				.value = globalState.nodeStates[nodeId]->uId,
				.toId = nodeId,
			};
			if (msg.type == MESSAGE_TYPE_NACK_EXPLORE) {
				globalState.nodeStates[nodeId]->waitListCount--;
				DEBUG("Node[%d]: WaitListCount: %d\n", nodeId, globalState.nodeStates[nodeId]->waitListCount);
			}
			globalState.nodeStates[nodeId]->children[msg.fromId] = 0;
			if (globalState.nodeStates[nodeId]->leaderId < msg.value) {
				globalState.nodeStates[nodeId]->leaderId = msg.value;
				globalState.nodeStates[nodeId]->parentId = msg.fromId;
				// send explore msg with parent nodeId and leader leaderId to all connected nodes
				DEBUG("Node[%d]: Send Explore to all nodes except %d\n", nodeId, msg.fromId);
				sendMsg.value = globalState.nodeStates[nodeId]->leaderId;
				for (childIndex = 0; childIndex < globalState.nProcess; childIndex++) {
					if (globalState.nodeStates[nodeId]->connectivity[childIndex] == 1 && childIndex != nodeId) {
						sendMsg.toId = childIndex;
						// TODO send ack after all explores have been acked or nacked
						if (msg.fromId == childIndex) {
							DEBUG("Node[%d]: Send ACK to %d\n", nodeId, sendMsg.toId);
							sendMsg.type = MESSAGE_TYPE_ACK;
							(globalState.nodeStates[nodeId]->stats).numAcks ++;
						} else {
							sendMsg.type = MESSAGE_TYPE_EXPLORE;
							// update message type based on the content of the message due to other incoming and processed messages
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
				// if message contains explore already, change it to nack and explore
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
			if (msg.type == MESSAGE_TYPE_ACK) {
				globalState.nodeStates[nodeId]->children[msg.fromId] = 1;
			} else {
				globalState.nodeStates[nodeId]->children[msg.fromId] = 0;
			}
			DEBUG("Node[%d]: WaitListCount: %d\n", nodeId, globalState.nodeStates[nodeId]->waitListCount);
		}
	}

	if (globalState.nodeStates[nodeId]->waitListCount == 0) {
		globalState.nodeStates[nodeId]->children[globalState.nodeStates[nodeId]->parentId] = 0;
	}
	pthread_mutex_unlock(&globalState.nodeStates[nodeId]->recvBufferMutex);
}

void consumeMessages(int nodeId) {
	int index = 0;
	pthread_mutex_lock(&globalState.nodeStates[nodeId]->recvBufferMutex);

	// remove messages from channel of communication and store in processBuffer
	// processBuffer will be used in generateMessages in the next round
	for (index = 0; index < globalState.nodeStates[nodeId]->recvBufferSize; index++) {
		logMessage(nodeId, globalState.nodeStates[nodeId]->recvBuffer[index]);
		memcpy(&globalState.nodeStates[nodeId]->processBuffer[index], &globalState.nodeStates[nodeId]->recvBuffer[index], sizeof(message_t));
	}

	// set received count
	globalState.nodeStates[nodeId]->processBufferSize = globalState.nodeStates[nodeId]->recvBufferSize;
	globalState.nodeStates[nodeId]->recvBufferSize = 0;

	pthread_mutex_unlock(&globalState.nodeStates[nodeId]->recvBufferMutex);
}
