#include <stdio.h>

int m;
int n;

typedef struct {
	int front;
	int back;
	int array[10];
}queue_st;

int queue_add(queue_st *queue, int value) {
	if ((queue->front+1)%10 == queue->back) {
		return -1;
	} else {
		queue->array[queue->front] = value;
		queue->front = (queue->front+1)%10;
		return 0;
	}
}

int queue_remove(queue_st *queue) {
	if (queue->front == queue->back) {
		return -1;
	} else {
		int value = queue->array[queue->back];
		queue->back = (queue->back + 1)%10;
		return value;
	}
}

void callDfs(int vertex, int matrix[][m], int *visited) {
	if (visited[vertex] == 1)
		return;
	visited[vertex] = 1;
	printf("DFS VISITED %d\n", vertex+1);

	int i = 0;
	int j = 0;

	for (j = 0; j < m; j++) {
		if (matrix[vertex][j] == 1) {
			for (i = 0; i < n; i++) {
				if (matrix[i][j] != 0 && i != vertex) {
					// found other side of edge	
					if (visited[i] == 0)
						callDfs(i, matrix, visited);
				}
			}
		}
	}
}

void callBfs(int vertex, int matrix[][m], int *visited, queue_st *queue) {
	int i = 0;
	int j = 0;

	queue_add(queue, vertex);
	int v = queue_remove(queue);
	while (v != -1) {
		if (visited[v] != 1) {
			visited[v] = 1;
			for (j = 0; j < m; j++) {
				if (matrix[v][j] == 1) {
					for (i = 0; i < n; i++) {
						if (matrix[i][j] != 0 && i != v) {
							// found other side of edge
							if (visited[i] == 0) {
								queue_add(queue, i);
							}
						}
					}
				}
			}
			printf("BFS VISITED %d\n", v+1);
		}
		v = queue_remove(queue);
	}
}

int main() {
	printf("Enter number of vertices:");
	scanf("%d", &n);
	printf("Enter number of edges:");
	scanf("%d", &m);

	int matrix[n][m];
	int visited[n];

	if (n == 0 || m == 0) {
		printf("Bad values\n");
		return -1;
	}

	int i = 0;
	int j = 0;
	for (i = 0; i < n; i++) {
		visited[i] = 0;
		for (j = 0; j < m; j++) {
			matrix[i][j] = 0; 
			printf("Enter incidence[%d][%d]: ", i, j);
			scanf("%d", &(matrix[i][j]));
		}
	}
	callDfs(0, matrix, visited);

	for (i = 0; i < n; i++) {
		visited[i] = 0;
	}
	queue_st queue;
	queue.front = 0;
	queue.back = 0;
	callBfs(0, matrix, visited, &queue);

	return 1;
}
