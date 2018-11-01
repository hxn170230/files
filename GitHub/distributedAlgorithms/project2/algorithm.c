#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "algorithm.h"
#include "simulator.h"
#include "messages.h"
#include "log.h"

static int processMessage(message_t *msg, int nodeId, int fromId);

static void logStateAndMessage(int nodeId) {

}

static void setState(int nodeId, STATUS_T status) {
	DEBUG("Node[%d]: Moving from State(%d) to State(%d)\n", nodeId, globalState.nodeStates[nodeId]->status, status);
	globalState.nodeStates[nodeId]->status = status;
}

static STATUS_T getState(int nodeId) {
	return globalState.nodeStates[nodeId]->status;
}

int algorithmEnd() {
	int nodeId = 0;
	for (nodeId = 0; nodeId < globalState.nProcess; nodeId++) {
		DEBUG("Node[%d]: uId(%d) leader(%d) algorithmEnd(%d)\n", nodeId, globalState.nodeStates[nodeId]->uId, globalState.nodeStates[nodeId]->leaderId,
				globalState.nodeStates[nodeId]->algorithmEnd);
		if (globalState.nodeStates[nodeId]->leaderId == globalState.nodeStates[nodeId]->uId &&
				globalState.nodeStates[nodeId]->algorithmEnd == 1) {
			return 1;	
		}
	}
	return 0;
}

void printStatistics() {
	int nodeId = 0;

	for (nodeId = 0; nodeId < globalState.nProcess; nodeId++) {
		INFO("Node[%d]: uId(%d) Parent(%d) Leader(%d) Component(%d) Level(%d)\n", nodeId,
				globalState.nodeStates[nodeId]->uId,
				globalState.nodeStates[nodeId]->parentId,
				globalState.nodeStates[nodeId]->leaderId,
				globalState.nodeStates[nodeId]->componentId,
				globalState.nodeStates[nodeId]->level);
	}
}

static int uniform_rv(int rangeLow, int rangeHigh) {
#if 1
	int myRand = (int)rand();
	int range = rangeHigh - rangeLow + 1; //+1 makes it [rangeLow, rangeHigh], inclusive.
	int myRand_scaled = (myRand % range) + rangeLow;
	if (rangeLow == rangeHigh && rangeHigh == 0) {
		return 0;
	}
	return myRand_scaled;
#else
	return 0;
#endif
}

static void sendMessage(message_t msg, int nodeId, int toId) {
	logSendMessage(nodeId, msg);
	addElementToQueue(&(globalState.nodeStates[toId]->recvQueue[nodeId]), msg);
}

static void create_message(message_t *msg, int nodeId, MESSAGE_TYPE type, int value, int toId) {
	memset(msg, 0, sizeof(message_t));
	AlgorithmData algoData = {
		.level = globalState.nodeStates[nodeId]->level,
		.componentId = globalState.nodeStates[nodeId]->componentId,
		.leaderId = globalState.nodeStates[nodeId]->leaderId,
	};
	MWOEData data = {
		.id1 = globalState.nodeStates[nodeId]->myMWOEData.id1,
		.id2 = globalState.nodeStates[nodeId]->myMWOEData.id2,
		.edgeWeight = globalState.nodeStates[nodeId]->myMWOEData.edgeWeight,
	};
	msg->delay = globalState.currentRound + uniform_rv(0, MAX_RAND_RANGE);
	INFO("Node[%d]: id1: %d id2: %d\n", nodeId, data.id1, data.id2);
	msg->type = type;
	msg->value = value;
	msg->fromId = nodeId;
	msg->toId = toId;
	msg->uId = globalState.nodeStates[nodeId]->uId;

	msg->algoData.level = algoData.level;
	msg->algoData.componentId = algoData.componentId;
	msg->algoData.leaderId = algoData.leaderId;

	msg->data.id1 = data.id1;
	msg->data.id2 = data.id2;
	msg->data.edgeWeight = data.edgeWeight;
}

static void forwardToChildren(int nodeId, int fromId, message_t *msg) {
	int index = 0;
	for (index = 0; index < globalState.nProcess; index++) {
		if (globalState.nodeStates[nodeId]->connectivity[index] != 0 &&
				globalState.nodeStates[nodeId]->spanningTreeConnectivity[index] == TREE_EDGE &&
				nodeId != index && fromId != index) {
			INFO("Node[%d] forward message to %d\n", nodeId, index);
			message_t mesg;
			memcpy(&mesg, msg, sizeof(message_t));
			mesg.toId = index;
			addElementToQueue(&globalState.nodeStates[index]->recvQueue[nodeId], mesg);
			if (msg->type == MESSAGE_TYPE_INITIATE) {
				// used for convergecast
				globalState.nodeStates[nodeId]->MWOEResponses++;
			}
		}
	}
	DEBUG("Node[%d]: Current MWOEResponses: %d\n", nodeId, globalState.nodeStates[nodeId]->MWOEResponses);
}

static void findMWOE(int nodeId) {
	int index = 0;
	int minEdgeIndex = MAX_INT;
	int minEdgeValue = MAX_INT;

	globalState.nodeStates[nodeId]->myMWOEData.id1 = MAX_INT;
        globalState.nodeStates[nodeId]->myMWOEData.id2 = MAX_INT;
        globalState.nodeStates[nodeId]->myMWOEData.edgeWeight = MAX_INT;

	for (index = 0; index < globalState.nProcess; index++) {
		if (globalState.nodeStates[nodeId]->connectivity[index] != 0 &&
				globalState.nodeStates[nodeId]->spanningTreeConnectivity[index] == BASIC_EDGE &&
				nodeId != index) {
			if (minEdgeValue > globalState.nodeStates[nodeId]->connectivity[index]) {
				minEdgeValue = globalState.nodeStates[nodeId]->connectivity[index];
				minEdgeIndex = index;
			}
		}
	}
	INFO("Node[%d]: Min Edge So far: %d value(%d)\n", nodeId, minEdgeIndex, minEdgeValue);
	if (minEdgeValue != MAX_INT)
		globalState.nodeStates[nodeId]->myMWOEData.id1 = nodeId;
	globalState.nodeStates[nodeId]->myMWOEData.id2 = minEdgeIndex;
	globalState.nodeStates[nodeId]->myMWOEData.edgeWeight = minEdgeValue;

	message_t msg;
	if (minEdgeIndex == MAX_INT) {
		minEdgeIndex = globalState.nodeStates[nodeId]->parentId;
		DEBUG("Node[%d]: Current MWOEResponses: %d\n", nodeId, globalState.nodeStates[nodeId]->MWOEResponses);
		if (globalState.nodeStates[nodeId]->MWOEResponses == 0) {
			create_message(&msg, nodeId, MESSAGE_TYPE_MWOEREPORT, FAILURE, globalState.nodeStates[nodeId]->parentId);
		} else {
			// wait for children
			return;
		}
	} else {
		create_message(&msg, nodeId, MESSAGE_TYPE_TESTMWOE, SUCCESS, minEdgeIndex);
		globalState.nodeStates[nodeId]->MWOEResponses += 1;
	}
	sendMessage(msg, nodeId, minEdgeIndex);
}

static void forwardToParent(MESSAGE_TYPE msgType, int fromId) {
	message_t msg;
	memset(&msg, 0, sizeof(message_t));
	create_message(&msg, fromId, msgType, SUCCESS, globalState.nodeStates[fromId]->parentId);
	sendMessage(msg, fromId, globalState.nodeStates[fromId]->parentId);
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
	if (d.id2 == MAX_INT) {
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
	// send MESSAGE_TYPE_MWOE_SEND_CONNECT to MWOE id1
	message_t msg;
	create_message(&msg, nodeId, MESSAGE_TYPE_MWOE_SEND_CONNECT, SUCCESS, globalState.nodeStates[nodeId]->myMWOEData.id1);
	if (nodeId == globalState.nodeStates[nodeId]->myMWOEData.id1) {
		sendMessage(msg, nodeId, nodeId);
	} else {
		forwardToChildren(nodeId, nodeId, &msg);
	}
}

static void startBroadcast(int nodeId) {
	message_t msg;
	create_message(&msg, nodeId, MESSAGE_TYPE_INITIATE, SUCCESS, nodeId);
	// forwardToChildren(nodeId, nodeId, &msg);
	sendMessage(msg, nodeId, nodeId);
}

static void sendTestMWOEResponse(int fromId, int toId, int status) {
	message_t msg;
	create_message(&msg, fromId, MESSAGE_TYPE_TESTMWOE_RESP, status, toId);
	sendMessage(msg, fromId, toId);
}

static void sendCombineComponents(int nodeId, message_t *mesg) {
	// send MESSAGE_TYPE_MWOECONNECT to id2 in myMWOEData
	message_t msg;
	/*if (globalState.nodeStates[nodeId]->myMWOEData.id2 != MAX_INT &&
			globalState.nodeStates[nodeId]->spanningTreeConnectivity[globalState.nodeStates[nodeId]->myMWOEData.id2] == TREE_EDGE) {
		INFO("Node[%d]: Already connected!\n", nodeId);
		return;
	}*/
	create_message(&msg, nodeId, MESSAGE_TYPE_MWOECONNECT, SUCCESS, globalState.nodeStates[nodeId]->myMWOEData.id2);
	sendMessage(msg, nodeId, globalState.nodeStates[nodeId]->myMWOEData.id2);
}

#if 0
static void changeParent(int nodeId) {
	message_t msg;
	create_message(&msg, nodeId, MESSAGE_TYPE_CHANGE_PARENT, SUCCESS, globalState.nodeStates[nodeId]->myMWOEData.id2);
	forwardToChildren(nodeId, globalState.nodeStates[nodeId]->parentId, &msg);
}
#endif

static void resetMWOEData(int nodeId) {
	globalState.nodeStates[nodeId]->myMWOEData.id1 = MAX_INT;
        globalState.nodeStates[nodeId]->myMWOEData.id2 = MAX_INT;
        globalState.nodeStates[nodeId]->myMWOEData.id2 = MAX_INT;
}

#define DEFER_MESSAGE 1
#define REMOVE_MESSAGE 0

static int processMessage(message_t *msg, int nodeId, int fromId) {
	int absorb = 0;
	int retVal = REMOVE_MESSAGE;

	logStateAndMessage(nodeId);
	switch(msg->type) {
		case MESSAGE_TYPE_INITIATE:
			globalState.nodeStates[nodeId]->leaderId = msg->algoData.leaderId;
			globalState.nodeStates[nodeId]->level = msg->algoData.level;
			globalState.nodeStates[nodeId]->componentId = msg->algoData.componentId;
			globalState.nodeStates[nodeId]->parentId = fromId;

			setState(nodeId, FINDING);
			if (globalState.nodeStates[nodeId]->MWOEResponses != 0) {
				DEBUG("Node[%d]: MWOEResponses(%d) not zero\n", nodeId, globalState.nodeStates[nodeId]->MWOEResponses);
			}
			globalState.nodeStates[nodeId]->MWOEResponses = 0;
			// forward this message to connected nodes except self and parent/leader
			forwardToChildren(nodeId, fromId, msg); 
			// start findMWOEProcess
			findMWOE(nodeId);
			break;
		case MESSAGE_TYPE_TESTMWOE:
			DEBUG("Node[%d]: TEST MWOE from %d\n", nodeId, msg->fromId);
			if (globalState.nodeStates[nodeId]->level >= msg->algoData.level &&
					globalState.nodeStates[nodeId]->componentId != msg->algoData.componentId) {
				// current level >= other and current component id != other
				sendTestMWOEResponse(nodeId, msg->fromId, SUCCESS);
			} else if (msg->algoData.componentId == globalState.nodeStates[nodeId]->componentId) {
				// component id == other 
				sendTestMWOEResponse(nodeId, msg->fromId, FAILURE);
			} else {
				// defer
				retVal = DEFER_MESSAGE;
			}
			break;
		case MESSAGE_TYPE_TESTMWOE_RESP:
			DEBUG("Node[%d]: TEST MWOE Response from %d\n", nodeId, msg->fromId);
			if (getState(nodeId) != FINDING) {
				DEBUG("Node[%d]: State not expected\n");
			}
			// check failure in msg->value
			if (msg->value == SUCCESS && globalState.nodeStates[nodeId]->componentId != msg->algoData.componentId) {
				if (nodeId != globalState.nodeStates[nodeId]->parentId) {
					globalState.nodeStates[nodeId]->MWOEResponses -= 1;
				}
				forwardToParent(MESSAGE_TYPE_MWOEREPORT, nodeId);
			} else {
				globalState.nodeStates[nodeId]->MWOEResponses -= 1;
				globalState.nodeStates[nodeId]->spanningTreeConnectivity[msg->fromId] = REJECTED_EDGE;
				resetMWOEData(nodeId);
				findMWOE(nodeId);
			}
			break;
		case MESSAGE_TYPE_MWOEREPORT:
			DEBUG("Node[%d]: MWOE Report from %d\n", nodeId, msg->fromId);
			// set MWOEResponses to MWOEResponses - 1
			globalState.nodeStates[nodeId]->MWOEResponses -= 1;
			DEBUG("Node[%d]: Current MWOEResponses: %d\n", nodeId, globalState.nodeStates[nodeId]->MWOEResponses);
			// set min(MWOEData and self.MWOEData) to MWOEData
			checkAndSetMWOEData(nodeId, msg);
			// if MWOEResponses == 0? send MWOEData to leader via parent
			if (globalState.nodeStates[nodeId]->MWOEResponses == 0) {
				if (nodeId == globalState.nodeStates[nodeId]->parentId && globalState.nodeStates[nodeId]->myMWOEData.id2 != MAX_INT) {
					sendStartMWOEConnect(nodeId);
				} else if (nodeId != globalState.nodeStates[nodeId]->parentId) {
					forwardToParent(MESSAGE_TYPE_MWOEREPORT, nodeId);
				} else {
					INFO("Node[%d]: Algorithm end\n", nodeId);
					globalState.nodeStates[nodeId]->algorithmEnd = 1;
				}
			}
			break;
		case MESSAGE_TYPE_MWOE_SEND_CONNECT:
			setState(nodeId, FOUND);
			// if toId == self? MWOEConnect to MWOEData
			if (nodeId == msg->toId) {
				sendCombineComponents(nodeId, msg);
			} else {
				// toId != self? forward MWOE_SEND_CONNECT to all connected nodes except parent
				forwardToChildren(nodeId, fromId, msg);
			}
			break;
		case MESSAGE_TYPE_MWOECONNECT:
			if (getState(nodeId) != FOUND) {
				// defer message
				DEBUG("Node[%d]: Connect received in %d state\n", nodeId, getState(nodeId));
				retVal = DEFER_MESSAGE;
			} else {
				MWOEData msgData = msg->data;
				if (globalState.nodeStates[nodeId]->myMWOEData.id1 == msgData.id2 &&
						globalState.nodeStates[nodeId]->myMWOEData.id2 == msgData.id1 &&
						globalState.nodeStates[nodeId]->myMWOEData.edgeWeight == msgData.edgeWeight) {
					INFO("Node[%d]: CONNECT received from %d\n", nodeId, msg->fromId);
					// merge or absorb based on level
					if (globalState.nodeStates[nodeId]->level == msg->algoData.level) {
						DEBUG("Node[%d]: Merge operation of %d %d\n", nodeId, nodeId, msg->fromId);
						globalState.nodeStates[nodeId]->level += 1;
						if (globalState.nodeStates[nodeId]->uId < msg->uId) {
							globalState.nodeStates[nodeId]->leaderId = msg->uId;
							globalState.nodeStates[nodeId]->componentId = msg->algoData.componentId;
							globalState.nodeStates[nodeId]->parentId = msg->fromId;
						}
					} else if (globalState.nodeStates[nodeId]->level > msg->algoData.level) {
						DEBUG("Node[%d]: Absorb operation of %d %d\n", nodeId, nodeId, msg->fromId);
						absorb = 1;
					} else {
						INFO("Node[%d]: Should not reach here\n", nodeId);
					}
					sendMWOEConnectResponse(nodeId, msg->fromId, SUCCESS);
					// set the value of edge to TREE_EDGE
					globalState.nodeStates[nodeId]->spanningTreeConnectivity[msg->fromId] = TREE_EDGE;
					// expect a response now
				} else {
					DEBUG("Node[%d]: self MWOE data not equal to other\n", nodeId);
					retVal = DEFER_MESSAGE;
				}
			}
			break;
		case MESSAGE_TYPE_MWOECONNECT_RESP:
			if (getState(nodeId) != FOUND) {
				DEBUG("Node[%d]: CONNECT Resp received in wrong state(%d)\n", nodeId, getState(nodeId));
			}
			if (msg->value == SUCCESS) {
				if (globalState.nodeStates[nodeId]->uId < msg->uId) {
					globalState.nodeStates[nodeId]->leaderId = msg->uId;
					globalState.nodeStates[nodeId]->parentId = msg->fromId;
					globalState.nodeStates[nodeId]->componentId = msg->algoData.componentId;
				}
				globalState.nodeStates[nodeId]->spanningTreeConnectivity[msg->fromId] = TREE_EDGE;
			} else {
				INFO("Node[%d]: Failed to connect components\n", nodeId);
			}
			if (globalState.nodeStates[nodeId]->parentId == nodeId) {
				startBroadcast(nodeId);
			}
			setState(nodeId, SLEEPING);
			break;
		case MESSAGE_TYPE_CHANGE_PARENT:
			break;
		default:
			DEBUG("Node[%d]: unknown message?\n", nodeId);
	}
	return retVal;
}

static void incrementBufferSize(int index) {
	pthread_mutex_lock(&(globalState.nodeStates[index]->recvSizeMutex));
	globalState.nodeStates[index]->recvBufferSize += 1;
	pthread_mutex_unlock(&(globalState.nodeStates[index]->recvSizeMutex));
}

int generateMessages(int nodeId) {
	int index = 0;
	int messagesCount = 0;
	int done = 0;

	if (globalState.currentRound == 1) {
		message_t msg;
		create_message(&msg, nodeId, MESSAGE_TYPE_INITIATE, SUCCESS, nodeId);
		DEBUG("Node[%d]: leader: %d msg leader: %d\n", nodeId, globalState.nodeStates[nodeId]->leaderId, msg.algoData.leaderId);
		sendMessage(msg, nodeId, nodeId);
		globalState.nodeStates[nodeId]->recvBufferSize = globalState.nProcess;
		return 0;
	}

	for (index = 0; index < globalState.nProcess; index++) {
		if (globalState.nodeStates[nodeId]->connectivity[index] != 0 || index == nodeId) {
			message_t *msg = NULL;
			do {
				msg = peekNextElement(&(globalState.nodeStates[nodeId]->processQueue[index]));
				if (msg != NULL) {
					// check self id == message toId
					// if not for self ? immediately forward with one added to delay
					if (msg->toId != nodeId) {
						// should never get here.
						INFO("Node[%d]: ***********SHOULD NEVER GET HERE*********");
						msg = getNextElement(&(globalState.nodeStates[nodeId]->processQueue[index]));
                                                // process messages
                                                if (processMessage(msg, nodeId, index) != REMOVE_MESSAGE) {
							addElementToQueue(&(globalState.nodeStates[nodeId]->deferQueue[index]), *msg);
						}
					} else if (msg->delay > globalState.currentRound) {
						done = 1;
				       } else {
						msg = getNextElement(&(globalState.nodeStates[nodeId]->processQueue[index]));
						// process messages
                                                if (processMessage(msg, nodeId, index) != REMOVE_MESSAGE) {
							DEBUG("Node[%d]: Defer message\n", nodeId);
							addElementToQueue(&(globalState.nodeStates[nodeId]->deferQueue[index]), *msg);
						}
				       }
                        	}
			} while (msg != NULL && !done);
		}
		incrementBufferSize(index);
		done = 0;
	}
	return messagesCount;
}

void consumeMessages(int nodeId) {
	// move messages to process queue
	message_t *msg = NULL;
	int index = 0;

	for (index = 0; index < globalState.nProcess; index ++) {
		msg = getNextElement(&(globalState.nodeStates[nodeId]->deferQueue[index]));
		while (msg != NULL) {
			logRecvMessage(nodeId, *msg);
			addElementToQueue(&(globalState.nodeStates[nodeId]->processQueue[index]), *msg);
			msg = getNextElement(&(globalState.nodeStates[nodeId]->deferQueue[index]));
		}
		msg = getNextElement(&(globalState.nodeStates[nodeId]->recvQueue[index]));
		while (msg != NULL) {
			logRecvMessage(nodeId, *msg);
			addElementToQueue(&(globalState.nodeStates[nodeId]->processQueue[index]), *msg);
			msg = getNextElement(&(globalState.nodeStates[nodeId]->recvQueue[index]));
		}
	}
	globalState.nodeStates[nodeId]->recvBufferSize = 0;
}
