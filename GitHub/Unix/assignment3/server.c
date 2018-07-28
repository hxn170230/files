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
#include <pthread.h>

#include "server.h"
#include "log.h"

// global state
static server_t server;

// closes the open sockets
static void closeAllSockets() {
	for (int i = 0; i < MAX_CLIENTS; i++) {
		// close only open connections
		if (server.clients[i].state == CONNECTION_STATE_CONNECTED) {
			close(server.clients[i].clientFd);
		}
	}
	// close main server socket
	if (server.serverFd >= 0)
		close(server.serverFd);
	// close log file
	if (server.logFd >= 0)
		close(server.logFd);
}

// signal handler for SIGKILL, SIGHUP and SIGTERM
static void signalHandler(int signal) {
	// write to log file
	// close all connections and threads
	LOG(server.logFd, "Signal: %d", signal);
	closeAllSockets();
	_exit(0);
}

// Index of unused client structure
static int getFreeIndex(client_t *clients) {
	for (int i = 0; i < MAX_CLIENTS; i++) {
		// check if current index is not being used
		if (clients[i].state == CONNECTION_STATE_INITIALIZED) {
			return i;
		}
	}
	return -1;
}

// Index of client structure for client socket
static int getIndex(client_t *clients, int clientFd) {
	for (int i = 0; i < MAX_CLIENTS; i++) {
		// check if current index's fd is same as client fd
		if (clients[i].clientFd == clientFd) {
			return i;
		}
	}
	return -1;
}

// process disconnect from client
static void processDisconnect(int clientFd) {
	int clientIndex = getIndex(server.clients, clientFd);
	if (clientIndex >= 0) {
		// reset client structure
		LOG(server.logFd, "[Thread %d] Closing socket(%d)", pthread_self(), clientFd);
		server.clients[clientIndex].state = CONNECTION_STATE_INITIALIZED;
		server.clients[clientIndex].clientFd = -1;
	} else {
		LOG(server.logFd, "Something wrong with client structures");
	}
	close(clientFd);
}

// execute command from client
static int executeCommand(char *data, char *output) {
	// create pipe
	int pipes[2] = {0};
	int result = pipe(pipes);
	if (result < 0) {
		LOG(server.logFd, "[Thread %d] Failed to create communication channel for child process", pthread_self());
		return -1;
	}
	// create child process
	pid_t childPid = fork();
	if (childPid == 0) {
		// close stdout and dupe write pipe to stdout
		// execute command in child process
		close(pipes[0]);
		char *command = strtok(data, " ");
		char *args[10];
		args[0] = command;
		char *token = strtok(NULL, " ");
		int i = 1;
		while (token) {
			args[i] = token;
			token = strtok(NULL, " ");
			i++;
		}
		result = dup2(pipes[1], STDOUT_FILENO);
		execvp(command, args);
	} else {
		// parent process wait for child and reads the data
		int stat = 0;
		close(pipes[1]);
		waitpid(childPid, &stat, 0);

		result = read(pipes[0], output, MAX_DATA_SIZE-1);
		LOG(server.logFd, "[Thread %d] CommandOutput: %s", pthread_self(), output);

		close(pipes[0]);
	}
	return 0;
}

// thread function to process client commands and sending responses
void * processClient(void *arg) {
	int clientFd = *(int*)arg;

	LOG(server.logFd, "[Thread %d] Waiting for client(%d)", pthread_self(), clientFd);

	while (1) {
		message_t command;
		message_t commandResult;
		memset(&command, 0, sizeof(command));
		memset(&commandResult, 0, sizeof(commandResult));

		// receive from client
		int result = recv(clientFd, &command, sizeof(command), 0);
		if (result <= 0) {
			LOG(server.logFd, "[Thread %d] Read failed with client(%d)", pthread_self(), clientFd);
			processDisconnect(clientFd);
			return NULL;
		}

		LOG(server.logFd, "[Thread %d] Received command(%d) len(%d) data(%s)", pthread_self(),
				command.messageId, command.length, command.data);

		if (command.messageId != MESSAGE_TYPE_COMMAND) {
			processDisconnect(clientFd);
			return NULL;
		} else {
			// process command
			result = executeCommand(command.data, commandResult.data);
	
			// send result
			commandResult.messageId = MESSAGE_TYPE_RESULT;
			if (result == 0) { 
				commandResult.length = strlen(commandResult.data);
			} else {
				commandResult.length = 0;
			}
			send(clientFd, &commandResult, sizeof(commandResult), 0);
		}

	}
	// clean up
	processDisconnect(clientFd);
	return NULL;
}

// process connection
static void processConnect(int clientFd) {
	int clientIndex = getFreeIndex(server.clients);
	if (clientIndex >= 0) {
		// store client information
		server.clients[clientIndex].clientFd = clientFd;
		server.clients[clientIndex].state = CONNECTION_STATE_CONNECTED;
		// create thread
		pthread_create(&(server.clients[clientIndex].threadId), NULL, processClient, &clientFd);
	} else {
		LOG(server.logFd, "MAX Clients reached..Cannot process any more");
		close(clientFd);
	}
}

int main(int argc, char *argv[]) {
	int portNumber = 0;
	int opt = 1;

	// usage
	if (argc != 2) {
		printf("Server needs port number!\n");
		return 1;
	}
	portNumber = atoi(argv[1]);

	// initialization
	server.serverFd = -1;
	server.logFd = -1;	
	for (int i = 0; i < MAX_CLIENTS; i++) {
		server.clients[i].state = CONNECTION_STATE_INITIALIZED;
		server.clients[i].clientFd = -1;
	}

	struct sockaddr_in addr = {
		.sin_family = AF_INET,
		.sin_port = htons(portNumber),
		.sin_addr.s_addr = htonl(INADDR_ANY),
	};

	// server socket
	server.serverFd = socket(PF_INET, SOCK_STREAM, 0);
	if (server.serverFd < 0) {
		printf("Unable to create socket\n");
		return 1;
	}

	setsockopt(server.serverFd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));

	// bind to any addr
	int result = bind(server.serverFd, (struct sockaddr *)&addr, sizeof(struct sockaddr));
	if (result < 0) {
		printf("Unable to bind socket to server: %s\n", strerror(errno));
		closeAllSockets();
		return 1;
	}

	printf("Server bound to %u\n", INADDR_ANY);

	// listen to the socket
	result = listen(server.serverFd, BACK_LOG);
	if (result < 0) {
		printf("Unable to listen to socket\n");
		close(server.serverFd);
		return 1;
	}

	// open the log file and register signal handlers to exit properly
	openLogFile(&(server.logFd), SERVER_LOG_FILE);
	signal(SIGINT, signalHandler);
        signal(SIGKILL, signalHandler);
        signal(SIGTERM, signalHandler);

	LOG(server.logFd, "Server Initialized");
	
	while (1) {
		// accept client connections
		struct sockaddr_in clientAddr;
		socklen_t len;
		int clientFd  = accept(server.serverFd, (struct sockaddr *)&clientAddr, &len);
		if (clientFd < 0) {
			LOG(server.serverFd, "Accept failed");
		} else {
			LOG(server.serverFd, "Accepted connection from (%s)", inet_ntoa(clientAddr.sin_addr));
			// create thread, process commands from client
			processConnect(clientFd);
		}
	}

	return 0;
}
