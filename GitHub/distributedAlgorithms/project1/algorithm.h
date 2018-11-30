#ifndef ALGORITHM_H

#define ALGORITHM_H

#include "messages.h"

void generateMessages(message_t *messages, int nodeId);
void consumeMessages(message_t *messages, int nodeId);

int algorithmEnd();

#endif
