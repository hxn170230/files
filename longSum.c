#include <stdio.h>

int longSum(int *a, int n)
{
	int max = 0;
	int sum[n][n];
	int i, j;
	for (i = 0; i < n; i++)
	{
		for (j = 0; j < n; j++)
		{
			if (i == j)
			{
				sum[i][j] = a[i];
			} else if (i > j) {
				sum[i][j] = sum[j][i];
			} else
				sum[i][j] = sum[i][j-1] + a[j];
			if (i == 0 && j == 0)
				max = sum[i][j];
			if (max < sum[i][j])
				max = sum[i][j];
			printf("sum[%d][%d] = %d\n", i, j, sum[i][j]);
		}
	}
	return max;
}

int main()
{
	int a[] = {1,-2,3, 5, -6, 1, 7, -8};
	printf("longSum = %d", longSum(a, 8));
	return 0;
}
