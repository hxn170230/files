#include <stdio.h>
int totalSwaps = 0;
int *merge(int *array1, int n, int *array2, int m) {
	int i = 0;
	int j = 0;
	int k = 0;
	int *array = (int *)malloc((n+m)*sizeof(int));
	while (i < n && j < m) {
		if (array1[i] < array2[j]) {
			array[k] = array1[i];
			i++;
		} else {
			totalSwaps++;
			printf("Adding %d\n", array2[j]);
			array[k] = array2[j];
			j++;
		}
		k++;
	}

	while (i < n) {
		array[k++] = array1[i++];
	}

	while (j < m) {
		array[k++] = array2[j++];
	}
	return array;
}

int *mergesort(int *array, int start, int end) {
	int mid = start + (end-start)/2;
	if (start == end) {
		int *temp = (int *)malloc(1*sizeof(int));
		temp[0] = array[start];
		return temp;
	} else if (mid == start) {
		int *temp = (int *)malloc(2*sizeof(int));
		if (array[start] < array[end]) {
			temp[0] = array[start];
			temp[1] = array[end];
		} else {
			totalSwaps++;
			printf("Swapping %d %d\n", array[start], array[end]);
			temp[0] = array[end];
			temp[1] = array[start];
		}
		return temp;
	} else {
		int *array1 = mergesort(array, start, mid-1);
		int i = 0;
		int *array2 = mergesort(array, mid, end);
		int *array3 = merge(array1, mid-start, array2, (end-mid)+1);
		return array3;
	}
}

int main() {
	int size = 7;
	int i = 0;
	int array[7] = {8, 6, 9, 5, 10, 7, 11};

	for (i = 0; i < size; i++) {
		printf("%d ",array[i]);
	}
	printf("\n");

	int *sortedArray = mergesort(array, 0, size-1);
	for (i = 0; i < size; i++) {
		printf("%d ", sortedArray[i]);
	}
	printf("\n");
	printf("Total swaps: %d\n", totalSwaps);
	return 0;
}
