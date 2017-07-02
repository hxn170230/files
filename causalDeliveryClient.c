#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/select.h>
#include <unistd.h>

#ifndef CLIENT_PORT
#define CLIENT_PORT 27000
#endif
#define SERVER_PORT 25000

int main() {
        unsigned int flags = IFF_BROADCAST|IFF_UP|IFF_RUNNING;
	struct sockaddr_in myAddr;
	struct sockaddr_in serverAddr;
	memset(&myAddr, 0, sizeof(struct sockaddr_in));
	myAddr.sin_family = AF_INET;
	myAddr.sin_port = htons(CLIENT_PORT);
	myAddr.sin_addr.s_addr = INADDR_ANY;

	memset(&serverAddr, 0, sizeof(struct sockaddr_in));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);

	struct ifaddrs *ifaddresses;
        if (!getifaddrs(&ifaddresses)) {
                struct ifaddrs *tmp = ifaddresses;
                while (tmp != NULL) {
                        if (tmp->ifa_addr &&
                                tmp->ifa_addr->sa_family == AF_INET &&
                                (tmp->ifa_flags&flags) == flags) {
                                struct sockaddr_in *pAddr = (struct sockaddr_in *)tmp->ifa_addr;
                                // found the right address
                                memcpy(&(serverAddr.sin_addr), &(pAddr->sin_addr), sizeof(struct in_addr));
                                printf("ifname: %s ifaddress: %s\n", tmp->ifa_name, inet_ntoa(serverAddr.sin_addr));
                        }
                        tmp = tmp->ifa_next;
                }
        } else {
                printf("Could not find any good address to connect to\n");
                freeifaddrs(ifaddresses);
                return -1;
        }
        freeifaddrs(ifaddresses);

	int mysocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (mysocket < 0) {
		printf("Unable to create socket\n");
		return -1;
	}

	unsigned int port = CLIENT_PORT;
	while (bind(mysocket, (struct sockaddr *)&myAddr, sizeof(struct sockaddr)) &&
			port < 65535) {
		printf("Bind failure on port %u\n", port);
		port++;
		myAddr.sin_port = htons(port);
	}

	if (connect(mysocket, (struct sockaddr *)&serverAddr, sizeof(struct sockaddr))) {
		printf("Unable to connect\n");
		perror("Unable to connect");
		return -1;
	}

	printf("Connection succeeded with server\n");
	char buffer[100] = {0,};
	int readBytes = recv(mysocket, buffer, 100, 0);
	printf("read %s from server\n", buffer);
	sleep(20);
	int sentBytes = send(mysocket, "Hello From Client", strlen("Hello From Client"), 0);
	if (sentBytes != strlen("Hello From Client")) {
		printf("Sent only %d\n", sentBytes);
	}

	close(mysocket);
	return 0;
}
