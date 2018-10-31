#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "algorithm.h"
#include "simulator.h"
#include "messages.h"
#include "log.h"

static void processMessage(message_t *msg, int nodeId, int fromId);

int algorithmEnd() {
	if (globalState.currentRound < MAX_ROUNDS) {
		return 0;
	}
	return 1;
}

void printStatistics() {

}

static int uniform_rv(int rangeLow, int rangeHigh) {
#if 0
	int myRand = (int)rand();
	int range = rangeHigh - rangeLow + 1; //+1 makes it [rangeLow, rangeHigh], inclusive.
	int myRand_scaled = (myRand % range) + rangeLow;
	if (rangeLow == rangeHigh && rangeHigh == 0) {
		return 0;
	}
	return myRand_scaled;
#endif
	return 0;
}

static void sendMessage(message_t msg, int nodeId, int toId) {
	logMessage(nodeId, msg);
	INFO("Node[%d]: Adding message to %d from %d\n", nodeId, toId, nodeId);
	addElementToQueue(&(globalState.nodeStates[toId]->recvQueue[nodeId]), msg);
}

static void create_message(message_t *msg, int nodeId, MESSAGE_TYPE type, int value, int toId) {
	memset(msg, 0, sizeof(message_t));
	AlgorithmData algoData = {
		.level = globalState.nodeStates[nodeId]->level,
		.componentId = globalState.nodeStates[nodeId]->componentId,
	};
	MWOEData data = {
		.id1 = globalState.nodeStates[nodeId]->myMWOEData.id1,
		.id2 = globalState.nodeStates[nodeId]->myMWOEData.id2,
		.edgeWeight = globalState.nodeStates[nodeId]->myMWOEData.edgeWeight,
	};
	INFO("Node[%d]: id1: %d id2: %d\n", nodeId, data.id1, data.id2);
	msg->type = type;
	msg->value = value;
	msg->fromId = nodeId;
	msg->toId = toId;
	msg->uId = globalState.nodeStates[nodeId]->uId;
	msg->delay = globalState.currentRound + uniform_rv(0, MAX_RAND_RANGE);
	msg->algoData.level = algoData.level;
	msg->algoData.componentId = algoData.componentId;
	msg->data.id1 = data.id1;
	msg->data.id2 = data.id2;
	msg->data.edgeWeight = data.edgeWeight;
}

static void forwardToNodes(int nodeId, int fromId, message_t *msg) {
	int index = 0;
	for (index = 0; index < globalState.nProcess; index++) {
		if (globalState.nodeStates[nodeId]->connectivity[index] != 0 &&
				globalState.nodeStates[nodeId]->children[index] == SAME_COMPONENT &&
				nodeId != index && fromId != index) {
			INFO("Node[%d] forward message to %d\n", nodeId, index);
			msg->delay += 1;
			addElementToQueue(&globalState.nodeStates[index]->recvQueue[nodeId], *msg);
			globalState.nodeStates[nodeId]->MWOEResponses++;
		}
	}
}

static void findMWOE(int nodeId) {
	int index = 0;
	int minEdgeIndex = 2147483647;
	int minEdgeValue = 2147483647;

	globalState.nodeStates[nodeId]->myMWOEData.id1 = 2147483647;
        globalState.nodeStates[nodeId]->myMWOEData.id2 = 2147483647;
        globalState.nodeStates[nodeId]->myMWOEData.id2 = 2147483647;

	for (index = 0; index < globalState.nProcess; index++) {
		if (globalState.nodeStates[nodeId]->connectivity[index] != 0 &&
				globalState.nodeStates[nodeId]->children[index] != 1 &&
				globalState.nodeStates[nodeId]->spanningTreeConnectivity[index] == OTHER_COMPONENT &&
				nodeId != index) {
			INFO("Node[%d]: Min %d %d %d\n", nodeId, nodeId, index, minEdgeIndex);
			if (minEdgeValue > globalState.nodeStates[nodeId]->connectivity[index]) {
				minEdgeValue = globalState.nodeStates[nodeId]->connectivity[index];
				minEdgeIndex = index;
			}
		}
	}
	INFO("Node[%d]: Min Edge So far: %d value(%d)\n", nodeId, minEdgeIndex, minEdgeValue);
	if (minEdgeValue != 2147483647)
		globalState.nodeStates[nodeId]->myMWOEData.id1 = nodeId;
	globalState.nodeStates[nodeId]->myMWOEData.id2 = minEdgeIndex;
	globalState.nodeStates[nodeId]->myMWOEData.edgeWeight = minEdgeValue;

	message_t msg;
	if (minEdgeIndex == 2147483647) {
		minEdgeIndex = globalState.nodeStates[nodeId]->parentId;
		create_message(&msg, nodeId, MESSAGE_TYPE_MWOEDATA, FAILURE, globalState.nodeStates[nodeId]->parentId);
	} else {
		create_message(&msg, nodeId, MESSAGE_TYPE_TESTMWOE, SUCCESS, minEdgeIndex);
	}
	sendMessage(msg, nodeId, minEdgeIndex);
	globalState.nodeStates[nodeId]->MWOEResponses += 1;
	INFO("Node[%d] %d sent to %d\n", nodeId, MESSAGE_TYPE_TESTMWOE, minEdgeIndex);
}

static void sendDataToParent(MESSAGE_TYPE msgType, int fromId) {
	if (fromId == globalState.nodeStates[fromId]->parentId) {
		INFO("Node[%d]: Error sending data to parent\n", fromId);
	} else {
		message_t msg;
		memset(&msg, 0, sizeof(message_t));
		create_message(&msg, fromId, msgType, SUCCESS, globalState.nodeStates[fromId]->parentId);
		sendMessage(msg, fromId, globalState.nodeStates[fromId]->parentId);
	}
}

static int compare(MWOEData data1, MWOEData data2) {
	if (data1.edgeWeight < data2.edgeWeight) {
		return 0;
	} else if (data1.edgeWeight > data2.edgeWeight) {
		return 1;
	} else {
		if (data1.id1 < data2.id1) {
			return 0;
		}
		return 1;
	}
}

static void checkAndSetMWOEData(int nodeId, message_t *msg) {
	MWOEData d = msg->data;
	if (d.id2 == 2147483647) {
		INFO("Node[%d]: Algorithm ended for %d\n", nodeId, msg->fromId);
	}
	if (compare(globalState.nodeStates[nodeId]->myMWOEData, msg->data) == 1) {
		memcpy(&(globalState.nodeStates[nodeId]->myMWOEData), &d, sizeof(MWOEData));
	}
}

static void sendMWOEConnectResponse(int fromId, int toId, int status) {
	message_t msg;
        create_message(&msg, fromId, MESSAGE_TYPE_MWOECONNECT_RESP, status, toId);
	sendMessage(msg, fromId, toId);
}

static void sendStartMWOEConnect(int nodeId) {
	// send MESSAGE_TYPE_MWOE_START_CONNECT to MWOE id1
	message_t msg;
	create_message(&msg, nodeId, MESSAGE_TYPE_MWOE_START_CONNECT, SUCCESS, globalState.nodeStates[nodeId]->myMWOEData.id1);
	if (nodeId == globalState.nodeStates[nodeId]->myMWOEData.id1) {
		sendMessage(msg, nodeId, nodeId);
	} else {
		forwardToNodes(nodeId, nodeId, &msg);
	}
}

static void startBroadcast(int nodeId) {
	message_t msg;
	create_message(&msg, nodeId, MESSAGE_TYPE_BROADCAST, SUCCESS, nodeId);
	// forwardToNodes(nodeId, nodeId, &msg);
	globalState.nodeStates[nodeId]->status = BROADCAST_INITIATED;
	sendMessage(msg, nodeId, nodeId);
}

static void sendTestMWOEResponse(int fromId, int toId, int status) {
	message_t msg;
	create_message(&msg, fromId, MESSAGE_TYPE_TESTMWOE_RESP, status, toId);
	sendMessage(msg, fromId, toId);
}

static void combineComponents(int nodeId, message_t *mesg) {
	// send MESSAGE_TYPE_MWOECONNECT to id2 in myMWOEData
	message_t msg;
	create_message(&msg, nodeId, MESSAGE_TYPE_MWOECONNECT, SUCCESS, globalState.nodeStates[nodeId]->myMWOEData.id2);
	sendMessage(msg, nodeId, globalState.nodeStates[nodeId]->myMWOEData.id2);
	globalState.nodeStates[nodeId]->status = CONNECT_SENT;
}

static void processMessage(message_t *msg, int nodeId, int fromId) {
	int absorb = 0;
	switch(msg->type) {
		case MESSAGE_TYPE_BROADCAST:
			// set parent id, leader id, level, component id
			globalState.nodeStates[nodeId]->parentId = fromId;
			globalState.nodeStates[nodeId]->level = msg->algoData.level;
			globalState.nodeStates[nodeId]->componentId = msg->algoData.componentId;
			globalState.nodeStates[nodeId]->leaderId = msg->uId;
			INFO("Node[%d]: level: %d componentId: %d leader: %d\n", nodeId, msg->algoData.level, msg->algoData.componentId, msg->fromId);

			// set MWOEResponses to 0
			globalState.nodeStates[nodeId]->MWOEResponses = 0;
			// forward this message to connected nodes except self and parent/leader
			forwardToNodes(nodeId, fromId, msg); 
			// start findMWOEProcess
			findMWOE(nodeId);
			break;
		case MESSAGE_TYPE_TESTMWOE:
			INFO("Node[%d]: Test MWOE from %d\n", nodeId, msg->fromId);
			if (globalState.nodeStates[nodeId]->level >= msg->algoData.level &&
					globalState.nodeStates[nodeId]->componentId != msg->algoData.componentId) {
				// accept TESTMWOE
				sendTestMWOEResponse(nodeId, msg->fromId, SUCCESS);
			} else if (globalState.nodeStates[nodeId]->componentId == msg->algoData.componentId) {
				// reject TESTMWOE
				sendTestMWOEResponse(nodeId, msg->fromId, FAILURE);
			} else {
				// defer reply
			}
			break;
		case MESSAGE_TYPE_TESTMWOE_RESP:
			// resp == success?
			globalState.nodeStates[nodeId]->MWOEResponses -= 1;
			if (msg->value == SUCCESS) {
				if (globalState.nodeStates[nodeId]->MWOEResponses == 0) {
					if (nodeId == globalState.nodeStates[nodeId]->parentId) {
						sendStartMWOEConnect(nodeId);
					} else {
						sendDataToParent(MESSAGE_TYPE_MWOEDATA, nodeId);
					}
				} else {
					// wait for responses from children
					INFO("Node[%d]: MWOEResponses: %d\n", globalState.nodeStates[nodeId]->MWOEResponses);
				}
			} else {
			// resp == failure?
			// 	continue findMWOEProcess
				if (globalState.nodeStates[nodeId]->myMWOEData.id2 != 2147483647) {
					globalState.nodeStates[nodeId]->spanningTreeConnectivity[globalState.nodeStates[nodeId]->myMWOEData.id2] = REJECTED_EDGE;
				}
				INFO("Node[%d]: Failed to test MWOE with %d\n", nodeId, msg->fromId);
				findMWOE(nodeId);
			}
			break;
		case MESSAGE_TYPE_MWOEDATA:
			// set MWOEResponses to MWOEResponses - 1
			globalState.nodeStates[nodeId]->MWOEResponses -= 1;
			// set min(MWOEData and self.MWOEData) to MWOEData
			checkAndSetMWOEData(nodeId, msg);
			// if MWOEResponses == 0? send MWOEData to leader via parent
			if (globalState.nodeStates[nodeId]->MWOEResponses == 0) {
				if (nodeId == globalState.nodeStates[nodeId]->parentId && globalState.nodeStates[nodeId]->myMWOEData.id2 != 2147483647) {
					sendStartMWOEConnect(nodeId);
				} else if (nodeId != globalState.nodeStates[nodeId]->parentId) {
					sendDataToParent(MESSAGE_TYPE_MWOEDATA, nodeId);
				} else {
					INFO("Node[%d]: Algorithm end\n", nodeId);
				}
			}
			break;
		case MESSAGE_TYPE_MWOE_START_CONNECT:
			// if toId == self? MWOEConnect to MWOEData
			if (nodeId == msg->toId) {
				INFO("Node[%d]: Connect request received\n", nodeId);
				INFO("Node[%d]: Combining %d %d\n", nodeId, globalState.nodeStates[nodeId]->myMWOEData.id1, globalState.nodeStates[nodeId]->myMWOEData.id2);
				combineComponents(nodeId, msg);
			} else {
				// toId != self? forward MWOE_START_CONNECT to all connected nodes except parent
				forwardToNodes(nodeId, fromId, msg);
			}
			break;
		case MESSAGE_TYPE_MWOECONNECT:
			INFO("Node[%d] MWOE Connect received from %d\n", nodeId, msg->fromId);
			if (globalState.nodeStates[nodeId]->level > msg->algoData.level) {
				// absorb operation
				INFO("Node[%d]: Absorb operation of %d\n", nodeId, msg->fromId);
				absorb = 1;
			} else {
				// merge operation
				if (globalState.nodeStates[nodeId]->level < msg->algoData.level) {
					INFO("Node[%d]*************WRONG MERGE************\n", nodeId);
				} else {
					INFO("Node[%d]: Merge operation of %d\n", nodeId, msg->fromId);
					globalState.nodeStates[nodeId]->level += 1;
				}
			}
			if (!absorb) {
				if (msg->uId < globalState.nodeStates[nodeId]->uId) {
					globalState.nodeStates[nodeId]->parentId = nodeId;
					globalState.nodeStates[nodeId]->leaderId = globalState.nodeStates[nodeId]->uId;
					globalState.nodeStates[nodeId]->children[msg->fromId] = SAME_COMPONENT;	
				} else {
					globalState.nodeStates[nodeId]->parentId = msg->fromId;
					globalState.nodeStates[nodeId]->leaderId = msg->uId;
					globalState.nodeStates[nodeId]->children[msg->fromId] = SAME_COMPONENT;	
					globalState.nodeStates[nodeId]->componentId = msg->algoData.componentId;
				}
			}
			globalState.nodeStates[nodeId]->spanningTreeConnectivity[msg->fromId] = SAME_COMPONENT;	
			sendMWOEConnectResponse(nodeId, msg->fromId, SUCCESS);
			// if new leader == self? start broadcast
			if (globalState.nodeStates[nodeId]->leaderId == globalState.nodeStates[nodeId]->uId)
				startBroadcast(nodeId);
			INFO("Node[%d]: New leader %d ComponentId: %d\n", nodeId, globalState.nodeStates[nodeId]->leaderId, globalState.nodeStates[nodeId]->componentId);
			break;
		case MESSAGE_TYPE_MWOECONNECT_RESP:
			INFO("Node[%d] MWOE Connect Resp received from %d\n", nodeId, msg->fromId);
			// leader is either self or other end
			globalState.nodeStates[nodeId]->spanningTreeConnectivity[msg->fromId] = SAME_COMPONENT;	
			// if new leader == self? start broadcast
			globalState.nodeStates[nodeId]->level = msg->algoData.level;
			if (msg->algoData.componentId != globalState.nodeStates[nodeId]->componentId) {
				globalState.nodeStates[nodeId]->leaderId = msg->uId;
				globalState.nodeStates[nodeId]->parentId = msg->fromId;
			}
			globalState.nodeStates[nodeId]->componentId = msg->algoData.componentId;
			if (globalState.nodeStates[nodeId]->leaderId == globalState.nodeStates[nodeId]->uId && 
					globalState.nodeStates[nodeId]->status != BROADCAST_INITIATED)
				startBroadcast(nodeId);
			INFO("Node[%d]: New leader %d ComponentId: %d\n", nodeId, globalState.nodeStates[nodeId]->leaderId, globalState.nodeStates[nodeId]->componentId);
			break;
		default:
			DEBUG("Node[%d]: unknown message?\n", nodeId);
	}
}

int generateMessages(int nodeId) {
	int index = 0;
	int messagesCount = 0;
	int done = 0;

	if (globalState.currentRound == 1) {
		message_t msg;
		create_message(&msg, nodeId, MESSAGE_TYPE_BROADCAST, SUCCESS, nodeId);
		sendMessage(msg, nodeId, nodeId);
		globalState.nodeStates[nodeId]->recvBufferSize = globalState.nProcess;
		return 0;
	}

	for (index = 0; index < globalState.nProcess; index++) {
		if (index == nodeId) {
			message_t *msg = NULL;
                        do {
                                msg = peekNextElement(&(globalState.nodeStates[nodeId]->processQueue[index]));
                                if (msg != NULL) {
                                        // check self id == message toId
                                        // if not for self ? immediately forward with one added to delay
                                        if (msg->delay <= globalState.currentRound) {
                                                msg = getNextElement(&(globalState.nodeStates[nodeId]->processQueue[index]));
                                                // process messages
                                                processMessage(msg, nodeId, index);
                                        } else if (msg->delay > globalState.currentRound) {
                                                logMessage(nodeId, *msg);
                                                DEBUG("Node[%d]: message(delay: %d) not be processed now!\n", nodeId, msg->delay);
                                                done = 1;
                                       }
                                }
                        } while (msg != NULL && !done);
		} else if (globalState.nodeStates[nodeId]->connectivity[index] != 0 && index != nodeId) {
			message_t *msg = NULL;
			do {
				msg = peekNextElement(&(globalState.nodeStates[nodeId]->processQueue[index]));
				if (msg != NULL) {
					// check self id == message toId
					// if not for self ? immediately forward with one added to delay
					if (msg->toId != nodeId) {
						msg = getNextElement(&(globalState.nodeStates[nodeId]->processQueue[index]));
                                                // process messages
                                                processMessage(msg, nodeId, index);
					} else if (msg->delay > globalState.currentRound) {
				   		logMessage(nodeId, *msg);
                               			DEBUG("Node[%d]: message(delay: %d) not be processed now!\n", nodeId, msg->delay);
						done = 1;
				       } else {
						msg = getNextElement(&(globalState.nodeStates[nodeId]->processQueue[index]));
						// process messages
						processMessage(msg, nodeId, index);
				       }
                        	}
			} while (msg != NULL && !done);
		}
		INFO("Node[%d]: index: %d size: %d\n", nodeId, index, globalState.nodeStates[index]->recvBufferSize);
		globalState.nodeStates[index]->recvBufferSize += 1;
		done = 0;
	}
	INFO("Node[%d]: messages %d\n", nodeId, messagesCount);
	return messagesCount;
}

void consumeMessages(int nodeId) {
	// move messages to process queue
	message_t *msg = NULL;
	int index = 0;

	for (index = 0; index < globalState.nProcess; index ++) {
		INFO("Node[%d]: Received messages!\n", nodeId);
		msg = getNextElement(&(globalState.nodeStates[nodeId]->recvQueue[index]));
		while (msg != NULL) {
			logMessage(nodeId, *msg);
			addElementToQueue(&(globalState.nodeStates[nodeId]->processQueue[index]), *msg);
			msg = getNextElement(&(globalState.nodeStates[nodeId]->recvQueue[index]));
		}
	}
	globalState.nodeStates[nodeId]->recvBufferSize = 0;
}
