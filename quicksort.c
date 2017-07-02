#include <stdio.h>

int quick(int * array, int i, int j)
{
	int starti = i;
	int endj = j;
	int pivoti = starti + (endj-starti)/2;
	int temp;
	int ret = 1;

	if ((i > j) || pivoti < i || !array)
		return 0;

	if (i == j)
		return 1;

	if (j-i == 1)
	{
		if (array[i] > array[j])
		{
			temp = array[i];
			array[i] = array[j];
			array[j] = temp;
		}
		return 1;
	}

	while (starti < endj)
	{
		if (array[starti] > array[endj])
		{
			temp = array[starti];
			array[starti] = array[endj];
			array[endj] = temp;
			endj--;
		} else {
			starti++;
		}
	}

	//found pivot value location.
	if (pivoti> i)
		ret = quick(array, i, pivoti);
	if (pivoti< j)
		ret = ret & quick(array, pivoti+1, j);
	return ret;
}

int quicksort(int *array, int length)
{
	return quick(array, 0, length-1);
}

void print(int *array, int length)
{
	int i = 0;
	printf("array: ");
	for (i = 0; i < length; i++)
	{
		printf("%d ", array[i]);
	}
	printf("\n");
}

int main()
{
	int array[] = {5,6,4,-1,0,1,3};
	int length = 7;

	quicksort(array, length-1);
	print(array, length);
	return 0;
}
