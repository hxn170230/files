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
#include <net/ethernet.h>

int main() {
	int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_IP);

	if (sockfd < 0) {
		perror("Failed to create socket");
		return -1;
	}

	char buffer[100] = {'\0',};
	int length = recvfrom(sockfd, buffer, 100, 0, NULL, 0);
	printf("Length of received message: %d\n", length);
	perror("Failed to receive");
	return 1;
}
