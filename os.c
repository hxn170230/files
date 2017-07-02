#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>

int main() {
	int comm[2];
	int child_comm[2];
	int ret = pipe(comm);
	ret = pipe(child_comm);
	if (ret < 0) {
		printf("Error: %d\n", errno);
		return -1;
	}

	pid_t child_pid = fork();
	if (child_pid < 0) {
		printf("Fork error: %d\n", errno);
		return -1;
	} else if (child_pid > 0) {
		char instruction[100] = {0, };
		close(comm[1]);
		close(child_comm[0]);
		waitpid(child_pid, &ret, WSTOPPED);
		printf("Return from child: %d\n", ret);
		if (ret == 0) {
			read(comm[0], instruction, 100);
			if (strcmp("Hello Parent", instruction) == 0) {
				printf("Message from child: %s\n", instruction);
				write(child_comm[1], "Hello Child", strlen("Hello Child"));
			}
		}
		waitpid(child_pid, &ret, 0);
		printf("Child return: %d\n", ret);
	} else {
		char instruction[100] = {0, };
		printf("Child started\n");
		pid_t parent = getppid();
		close(child_comm[1]);
		close(comm[0]);
		write(comm[1], "Hello Parent", strlen("Hello Parent"));
		//sleep(1);
		read(child_comm[0], instruction, 100);
		printf("Message from parent: %s\n", instruction);
		printf("Child done\n");
		_Exit(0);
	}

	return 0;
}
