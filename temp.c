#include <stdio.h>
#include <stdlib.h>

void merge(int *a, int n, int *b, int m, int *c)
{
	int index_c = 0;
	int index_a = 0;
	int index_b = 0;

	while (index_a < n && index_b < m) {
		if (a[index_a] <= b[index_b]) {
			c[index_c] = a[index_a];
			index_c += 1;
			index_a += 1;
		} else if (a[index_a] > b[index_b]) {
			c[index_c] = b[index_b];
			index_c += 1;
			index_b += 1;
		}
	}

	while (index_a < n)
	{
		c[index_c++] = a[index_a++];
	}

	while (index_b < m)
	{
		c[index_c++] = b[index_b++];
	}
}

void print(int *a, int num, char *result)
{
	int i = 0;
	printf("Array: %s\n", result);
	for (i = 0; i < num; i++)
	{
		printf("%d ", a[i]);
	}
	printf("\n");
}

int *mergeSort(int *a, int i, int j)
{
	int *result1;
	int *result2;
	int *c = (int *)malloc(j-i + 1);
	if (i == j)
	{
		c[j-i] = a[i];
		return c;
	}

	if ((j-i) == 1)
	{
		if (a[i] > a[j])
		{
			c[0] = a[j];
			c[1] = a[i];
		} else {
			c[0] = a[i];
			c[1] = a[j];
		}
		return c;
	}
			
	result1 = mergeSort(a, i, i+(j-i)/2);
	print(result1, (j-i)/2 + 1, "result1");
	result2 = mergeSort(a, (j-i)/2+1+i, j);
	print(result2, (j - (((j-i)/2)+1+i))+1, "result2");
	merge(result1, (j-i)/2+1, result2, (j - (((j-i)/2)+1+i))+1, c);
	print(c, j-i+1, "Result3");
	return c;
}

int main ()
{
	int a[] = {1, 2, -7 , 5, 10, 6, -9, 8, 11, 23, 12, 10, 7, 9};
	int n = 14;
	mergeSort(a, 0, n-1);
}
