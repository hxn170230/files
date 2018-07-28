#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>

#include "client.h"
#include "log.h"

// to add more clients, use interactive shell
// client 1 data
static char *commands1[] = {
	"date",
	"ls -l junk.txt",
	"touch junk.txt",
};

// client 2 data
static char *commands2[] = {
	"date",
	"ls -l junk.txt",
	"rm -rf junk.txt",
};

// client 3 data
static char *commands3[] = {
	"date",
	"ls -l junk.txt",
	"uname -a",
};

static char **commands[] = {
	commands1,
	commands2,
	commands3,
};	

// global state
static client_t client;

// close open sockets
void closeAllSockets(client_t client) {
        if (client.serverFd >= 0)
                close(client.serverFd);
        if (client.logFd >= 0)
                close(client.logFd);
}

// signal handler for SIGKILL, SIGHUP, and SIGTERM
void signalHandler(int signal) {
        // write to log file
        // close all connections and threads
	LOG(client.serverFd, "Received signal: %d", signal);
        closeAllSockets(client);
	LOG(client.serverFd, "Exiting");
        _exit(0);
}

int main(int argc, char *argv[]) {
        int portNumber = 0;

	char *serverAddr = NULL;
	char logFile[100] = {0};

        if (argc != 4) {
                printf("Client needs server ip, server port number and client id!\n");
                return 1;
        }
	serverAddr = argv[1];
        portNumber = atoi(argv[2]);

        client.serverFd = -1;
        client.logFd = -1;
	client.clientId = atoi(argv[3]);
	sprintf(logFile, "Client%dLog.txt", client.clientId);

        struct sockaddr_in addr = {
                .sin_family = AF_INET,
                .sin_port = htons(portNumber),
        };

        int result = inet_aton(serverAddr, &addr.sin_addr);
        if (result < 0) {
                printf("inet_aton failed\n");
                return 1;
        }

        client.serverFd = socket(PF_INET, SOCK_STREAM, 0);
        if (client.serverFd < 0) {
                printf("Unable to create socket\n");
                return 1;
        }

        openLogFile(&(client.logFd), logFile);
        signal(SIGINT, signalHandler);
        signal(SIGKILL, signalHandler);
        signal(SIGTERM, signalHandler);

	LOG(client.logFd, "Initialized", client.clientId);

	result = connect(client.serverFd, (struct sockaddr *)&addr, sizeof(addr));
	if (result < 0) {
		closeAllSockets(client);
		printf("Unable to open connection to server\n");
		return 1;
	}

	LOG(client.logFd, "Connected to Server @[%s:%s]", argv[1], argv[2]);

	message_t command;
	message_t commandResult;

	int commandIndex = 0;
	while (1) {
		// below is for testing client 1, client 2 and client 3
		if (commandIndex == 0) {
			if (client.clientId == 0)
				sleep(2);
			else if (client.clientId == 1)
				sleep(3);
			else
				sleep(1);
		}
		memset(&command, 0, sizeof(command));
		memset(&commandResult, 0, sizeof(commandResult));

		// below is for interactive Shell based clients
		/*printf(">>");
		fflush(NULL);

		result = read(STDIN_FILENO, command.data, MAX_DATA_SIZE-1);
		if (result < 0) {
			LOG(client.logFd, "Unable to read from STDIN");
			closeAllSockets(client);
			_exit(1);
		}
		command.data[strlen(command.data)-1] = '\0';*/
		sprintf(command.data, commands[client.clientId-1][commandIndex], strlen(commands[client.clientId-1][commandIndex]));
		commandIndex = (commandIndex + 1)%3;
		LOG(client.logFd, "COMMAND: %s", command.data);

		if (!strncmp(command.data, "exit", 4)) {
			LOG(client.logFd, "Closing connections with Server");
			closeAllSockets(client);
			_exit(0);
		}
		//  send command
		command.messageId = MESSAGE_TYPE_COMMAND;
		command.length = strlen(command.data);

		result = send(client.serverFd, &command, sizeof(command), 0);
		if (result <= 0) {
			LOG(client.logFd, "Unable to send command to server");
			closeAllSockets(client);
			_exit(1);
		}
		//  recv command result
		result = recv(client.serverFd, &commandResult, sizeof(commandResult), 0);
		if (result <= 0) {
			LOG(client.logFd, "Unable to send command to server");
			closeAllSockets(client);
			_exit(1);
		}
		//  log result
		if (commandResult.length > 0)
			LOG(client.logFd, "%s", commandResult.data);
	}

	return 0;
}
