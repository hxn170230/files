#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#define REASON_TIMER 0
#define REASON_SYSTEM_CALL 1

#define PORT_INT 1
#define PORT_CHAR 2

#define LOG true;

//commChild[0] (Parent reads from child)
//commChild[1] (Child writes to Parent)
static int commChild[2];
//commParent[0] (Child reads from Parent)
//commParent[1] (Parent writes to Child)
static int commParent[2];

typedef enum {
	LOAD=1,
	LOADADDR,
	LOADIND,
	LOADIDXX,
	LOADIDXY,
	LOADSPX,
	STORE,
	GET,
	PUT,
	ADDX,
	ADDY,
	SUBX,
	SUBY,
	COPYTOX,
	COPYFROMX,
	COPYTOY,
	COPYFROMY,
	COPYTOSP,
	COPYFROMSP,
	JUMP,
	JUMPIFEQUAL,
	JUMPIFNOTEQUAL,
	CALL,
	RET,
	INCX,
	INCY,
	DECX,
	PUSH,
	POP,
	INT,
	IRET,
	END,
	MAX_INSTRUCTIONS
}INSTRUCTIONS;

// -1 for FAILURE
// 0 to exit on receipt of END
// 1 for SUCCESS

int readFromMMU(int addr) {
	// TODO: If not in previlege mode, system addr should not be read
	char sendBuffer[11] = {0, };
	sprintf(sendBuffer, "FETCH %d", addr);
	printf("%s\n", sendBuffer);
	int result = write(commParent[1], sendBuffer, strlen(sendBuffer));
	int value = 0;
	result = read(commChild[0], &value, sizeof(int));
	printf("Received: %d\n", value);
	return value;
}

int readNextInstruction(int PC, int *IR) {
	// fetch instruction pointed to by PC via memPipe
	// save this instruction onto IR
	int value = readFromMMU(PC);
	if (value > 0) {
		*IR = value;
		if (value == 50) {
			printf("Instruction END encountered! HALTING...");
			return 0;
		} else return 1;
	} else {
		return -1;
	}
}

// 0 for FAILURE
// 1 for SUCCESS
int processInstruction(int *PC, int *IR, int *SP, int *AC, int *X, int *Y) {
	int result = 1;
	switch(*IR) {
        case LOAD:
		// value = read address in *PC from childComm
		// *AC = value
		// *PC = *PC + 1
	break;
        case LOADADDR:
	break;
        case LOADIND:
	break;
        case LOADIDXX:
	break;
        case LOADIDXY:
	break;
        case LOADSPX:
	break;
        case STORE:
	break;
        case GET:
	break;
        case PUT:
	break;
        case ADDX:
	break;
        case ADDY:
	break;
        case SUBX:
	break;
        case SUBY:
	break;
        case COPYTOX:
	break;
        case COPYFROMX:
	break;
        case COPYTOY:
	break;
        case COPYFROMY:
	break;
        case COPYTOSP:
	break;
        case COPYFROMSP:
	break;
        case JUMP:
	break;
        case JUMPIFEQUAL:
	break;
        case JUMPIFNOTEQUAL:
	break;
        case CALL:
	break;
        case RET:
	break;
        case INCX:
	break;
        case INCY:
	break;
        case DECX:
	break;
        case PUSH:
	break;
        case POP:
	break;
        case INT:
	break;
        case IRET:
	break;
        case END:
	break;
	}
	return result;
}

void processInterrupt(int reason) {
	printf("Timer interrupt!\n");
}

int initMemory(int *memory) {
	FILE *fp = fopen("program.txt", "r");
	if (fp < 0) {
		// FILE open failure.
		return -1;
	}

	char buffer[100] = {0,};
	int addr_start = 0;
	while (fgets(buffer, 100, fp) != NULL) {
		char *value = strtok(buffer, " ");
		if (value[strlen(value)-1] == '\n') {
			value[strlen(value)-1] = '\0';
		}
		if (!strlen(value)) {
			memset(buffer, '\0', 100);
			value = NULL;
			continue;
		}
		if (value[0] == '.') {
			// start of timer or system memory
			int i = 0;
			addr_start = 0;
			while (i < strlen(&value[1])) {
				addr_start = 10*addr_start + (value[i] - '0');
				i++;
			}
			addr_start = atoi(&value[1]);
		} else {
			int instruction = 0;
			int i = 0;
			while (i < strlen(value)) {
				instruction = 10*instruction + (value[i]-'0');
				i++;
			}
			memory[addr_start++] = instruction; 
		}
		memset(buffer, '\0', 100);
		value = NULL;
	}
	fclose(fp);
	return 1;
}

int main(int argc, char *argv[]) {
	int SP = 999;
	int PC = 0;
	int AC = 0;
	int IR = 0;
	int X = 0;
	int Y = 0;
	int timerVal = 0;
	int instructionsExecuted = 0;

	// initialize self and memory management entity
	if (argc < 2) {
		printf("USAGE: <Executable> <timer value>\n");
		return 0;
	}
	int result = pipe(commChild);
	if (result < 0) {
		// FATAL Failure
	}

	result = pipe(commParent);
	if (result < 0) {
		// FATAL Failure
	}

	pid_t childPid = fork();
	if (childPid < 0) {
		// FATAL Failure
	} else if (childPid == 0) {
		// memory management unit
		// init MMU
		int *memory = (int *)malloc(2000*sizeof(int));
		if (memory == NULL) {
			//FATAL Failure
		}

		initMemory(memory);
		// wait for read from CPU
		char readBuffer[11] = {0,};
		while (read(commParent[0], readBuffer, 11) > 0) {
			readBuffer[10] = '\0';
			if (!strcmp(readBuffer, "END")) {
				printf("MMU: Halting...\n");
				break;
			}
			char *token = strtok(readBuffer, " ");
			token[strlen(token)-1] = '\0';
			if (strcmp(token, "FETCH")) {
				char *caddr = strtok(NULL, " ");
				int addr = atoi(caddr);
				// TODO: check for invalid memory access!
				write(commChild[1], &memory[addr], sizeof(int));
			}
			memset(readBuffer, '\0', 11);
		}
		close(commChild[0]);
		close(commChild[1]);
		close(commParent[0]);
		close(commParent[1]);
		free(memory);
		memory = NULL;
		printf("MMU: Halted\n");
		_exit(0);
	} else {
		// initialize timerVal
		timerVal = atoi(argv[1]);
		int stop = 0;// change this to 0 during final execution
		do {
			result = readNextInstruction(PC, &IR);
			PC = PC + 1;
			if (result == -1) {
				// close memory management and exit
			} else if (result == 0) {
				// end of program, release resources and exit
				stop = 1;
				printf("Received END instruction");
			} else {
				printf("Instruction received: %d\n", IR);
				if (!processInstruction(&PC, &IR, &SP, &AC, &X, &Y)) {
					// failed to process instruction, exit??
				}
				instructionsExecuted++;
				if (timerVal == instructionsExecuted) {
					// run timer interrupt routine
					processInterrupt(REASON_TIMER);
					instructionsExecuted = 0;
					// TODO: on IRET reset instructionsExecuted to 0
				}
			}
		} while(!stop);
		// TODO: define END as constant
		write(commParent[1], "END", strlen("END"));
		waitpid(childPid, &result, 0);
		printf("CPU: MMU Halted\n");
		close(commChild[0]);
		close(commChild[1]);
		close(commParent[0]);
		close(commParent[1]);
	}
	printf("CPU: Halted\n");
	return result;
}
