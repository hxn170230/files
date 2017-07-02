#include <stdio.h>
#include <limits.h>
#include <stdlib.h>

int priorityQueueInsert(int *array, int size) {
	int swap_needed = 1;
	int leaf = array[0];
	while (swap_needed && leaf != 1) {
		int parent = leaf/2;
		if (array[parent] < array[leaf]) {
			int temp = array[parent];
			array[parent] = array[leaf];
			array[leaf] = temp;
		} else {
			swap_needed = 0;
		}
		leaf = parent;
	}
	return 1;
}

int removeMax(int *array) {
	int size = array[0];
	int max = array[1]; // max element
	array[1] = array[size];
	size--;
	array[0] = size;

	// heapify the queue again
	int i = 1;
	int swapNeeded = 1;
	int replaceChild = i;
	while (swapNeeded && (2*i+1 <= size)) {
		int leftChild = 2*i;
		int rightChild = 2*i+1;
		if (array[leftChild] > array[rightChild]) {
			replaceChild = leftChild;
		} else {
			replaceChild = rightChild;
		}

		if (array[i] < array[replaceChild]) {
			int temp = array[i];
			array[i] = array[replaceChild];
			array[replaceChild] = temp;
		} else {
			swapNeeded = 0;
		}
		i = replaceChild;
	}

	return max;
}

int main() {
	int size = 25;
	int array[size];
	int array_size = 0;
	int i = 0;
	for (i = 0; i < size; i++) {
		array[i] = INT_MIN;
	}
	i = 1;
	while (i <= size) {
		int value = rand()%100;
		array[i] = value;
		array_size++;
		array[0] = array_size;
		if (i != 1) {
			if (!priorityQueueInsert(array, size-1)) {
				printf("Unsuccessful operation\n");
				i--;
			}
		}
		i++;
	}

	printf("Array length: %d\n", array[0]);
	for (i = 1; i <= size; i++) {
		printf("%d ", array[i]);
	}
	printf("\n");

	printf("\n");
	while (array[0] > 0) {
		int value = removeMax(array);
		printf("%d ", value);
	}
	printf("\n");
	return 0;
}
