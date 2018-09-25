#ifndef ALGORITHM_H

#define ALGORITHM_H

#include "messages.h"
// Below functions need to be implemented by the desired algorithm

// output messages to send from nodeId based on algorithm
void generateMessages(message_t *messages, int nodeId);

// remove messages from channel of communication
void consumeMessages(int nodeId);

// Any statistics needed to print/debug
void printStatistics();

// decides the end of algorithm. 1 for end and 0 for continue
int algorithmEnd();

#endif
