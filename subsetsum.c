#include<stdio.h>
#include<stdlib.h>

long int findsum(long int *array, long int *dupes, long int i, long int sum) {
	if (i < 0) {
		return 0;
	}
	//printf("i: %ld sum: %ld\n", i, sum);
	if (i == 0 || sum <= 0) {
		if (i == 0 && sum == array[i] && dupes[i] > 0) {
			dupes[i] -= 1;
			return 1;
		}
		return 0;
	}
	long int val = array[i];
	if (dupes[i] == 0 && i == 0) {
		//printf("Dupe used\n");
		return 0;
	}

	if (sum - val == 0 && dupes[i] > 0) {
		dupes[i]-=1;
		//printf("%ld reduced to %ld\n", i, dupes[i]);
		return 1;
	}

	if (sum == val*dupes[i] && dupes[i] > 0) {
		dupes[i] = 0;
		//printf("%ld reduced to %ld\n", i, dupes[i]);
		return 1;
	}

	// include val and call findsum
	//printf("including %ld\n", array[i]);
	long int retval = 0;
	int dupeschanged = 0;
	if (dupes[i] > 0) {
		dupes[i]-=1;
		dupeschanged = 1;
		//printf("dupes %ld set to %ld\n", i, dupes[i]);
		retval = findsum(array, dupes, i-1, sum-val);
	}
	if (retval == 0) {
		//printf("Sum not found including %ld\n", i);
		// exclude val and call function
		if (dupeschanged > 0)
			dupes[i]+=1;
		//printf("Trying to find sum without %ld\n", i);
		retval = findsum(array, dupes, i-1, sum);
	}
	//printf("found?%ld\n", retval);
	return retval;
}

void swap(long int *a, long int *b) {
	long int temp = *a;
	*a = *b;
	*b = temp;
}

void sort(long int *array, int num) {
	int i = 0;
	int j = 0;
	for (i = 0; i < num-1; i++) {
		for (j = i; j < num; j++) {
			if (array[i] > array[j]) {
				swap(&array[i], &array[j]);
			}
		}
	}
}

int main(){
	int num = 0;
	do {
		scanf("%d", &num);
		//printf("Num:%d\n", num);
		if (num == 0) {
			break;
		}
		long int array[num];
		long int dupes[num];
		int i = 0;

		for (i = 0; i < num; i++) {
			array[i] = 0;
			dupes[i] = 0;
			scanf("%ld", &array[i]);
			dupes[i] = 1;
		}

		sort(array, num);

		long int total_sum = 0;
		for (i = num-1; i >= 0; i--) {
			////printf("dupes[%d] = %ld\n",i , dupes[i]);
			if (dupes[i] <= 0) {
				continue;
			}
			//////printf("FINDING %ld, DUPES?%ld\n", array[i], dupes[i]);
			long int sum = array[i];
			long int found = findsum(array, dupes, i-1, sum);
			////printf("dupes[%d] = %ld\n",i , dupes[i]);
			if (found == 0 && dupes[i] > 0) {
				total_sum += array[i];
				//////printf("Added to sum. Total:%ld\n", total_sum);
			}
		}
		printf("%ld\n", total_sum);
	} while (num != 0);

	return 0;
}

