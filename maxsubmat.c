#include <stdio.h>

void print_array(int m, int n, int array[][n]) {
	int i = 0;
	int j = 0;
	for (i = 0; i < m; i++) {
		for (j = 0; j < n; j++) {
			printf("%d ", array[i][j]);
		}
		printf("\n");
	}
	printf("\n");
}

int main() {
	int m = 6;
	int n = 5;

	int i = 0;
	int j = 0;

	int array[m][n];
	int max[m][n];
	int maxv = 0;
	for (i = 0; i < m; i++) {
		for (j = 0; j < n; j++) {
			scanf("%d", &array[i][j]);
			max[i][j] = 0;
			if (i == 0 || j == 0) {
				max[i][j] = array[i][j];
				if (maxv < max[i][j]) {
					maxv = max[i][j];
				}
			} else {
				if (array[i-1][j-1] == 1 && array[i][j-1] == 1 &&
						array[i][j] == 1 && array[i-1][j] == 1) {
					max[i][j] = max[i-1][j-1] + 1;
					if (maxv < max[i][j]) {
						maxv = max[i][j];
					}
				} else {
					max[i][j] = array[i][j];
					if (maxv < max[i][j]) {
						maxv = max[i][j];
					}
				}
			}
		}
	}
	print_array(m, n, array);
	printf("\n");
	print_array(m, n, max);
	printf("MAX ARRAY: %d\n", maxv);
}
