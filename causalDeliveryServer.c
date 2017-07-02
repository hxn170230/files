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

#define PORT 25000
#define BACKLOG 10

int main() {
	unsigned int flags = IFF_BROADCAST|IFF_UP|IFF_RUNNING;
	struct sockaddr_in *my_addr = NULL;
	struct sockaddr_in clientAddr;
	struct ifaddrs *ifaddresses;

	my_addr = (struct sockaddr_in *)malloc(1*sizeof(struct sockaddr_in));
	memset(&clientAddr, 0, sizeof(struct sockaddr_in));
	memset(my_addr, 0, sizeof(struct sockaddr_in));

	if (my_addr ==  NULL) {
		printf("No space allocated!\n");
		return -1;
	}

	my_addr->sin_family = AF_INET;
	my_addr->sin_port = htons(PORT);

	if (!getifaddrs(&ifaddresses)) {
		struct ifaddrs *tmp = ifaddresses;
		while (tmp != NULL) {
			if (tmp->ifa_addr &&
				tmp->ifa_addr->sa_family == AF_INET &&
				(tmp->ifa_flags&flags) == flags) {
				struct sockaddr_in *pAddr = (struct sockaddr_in *)tmp->ifa_addr;
				// found the right address
				memcpy(&(my_addr->sin_addr), &(pAddr->sin_addr), sizeof(struct in_addr));
				printf("ifname: %s ifaddress: %s\n", tmp->ifa_name, inet_ntoa(my_addr->sin_addr));
			}
			tmp = tmp->ifa_next;
		}
	} else {
		printf("Could not find any good address to bind to\n");
		freeifaddrs(ifaddresses);
		free(my_addr);
		return -1;
	}
	freeifaddrs(ifaddresses);

	int sockfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sockfd < 0) {
		printf("Error creating socket\n");
		free(my_addr);
		return -1;
	}

	if (bind(sockfd, (struct sockaddr *)my_addr, sizeof(struct sockaddr_in))) {
		printf("Error binding to socket %d\n", sockfd);
		free(my_addr);
		return -1;
	}

	printf("Bound to socket\n");
	if (listen(sockfd, BACKLOG)) {
		printf("Error listening to socket %d\n", sockfd);
		free(my_addr);
		return -1;
	}

	printf("listening on socket\n");
	fd_set fds;
	FD_ZERO(&fds);
	FD_SET(sockfd, &fds);
	int fdsize = 1;
	while (select(FD_SETSIZE, &fds, NULL, NULL, NULL) != -1) {
		printf("return from select\n");
		if (FD_ISSET(sockfd, &fds)) {
			printf("Incoming connection ??\n");
			unsigned int size = sizeof(struct sockaddr);
			int clientFd = accept(sockfd, (struct sockaddr *)&clientAddr, &size);
			if (clientFd < 0) {
				printf("Error accepting connection..continue\n");
				continue;
			} else {
				int sentBytes = send(clientFd, "Hello From Server", strlen("Hello From Server"), 0);
				if (sentBytes != strlen("Hello From Server")) {
					printf("Error sending from server\n");
				}
				char buffer[100] = {0,};
				int recvBytes = recv(clientFd, buffer, 100, 0);
				printf("received bytes: %s\n", buffer);
				close(clientFd);
			}
		}
	}

	FD_ZERO(&fds);
	close(sockfd);
	free(my_addr);
	return 0;
}
