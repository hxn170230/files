#include <stdio.h>

int main()
{
    int testCases = 0;
    scanf("%d", &testCases);
    int index = 0;
    int number[testCases];
    for (index = 0; index < testCases; index++) {
    	scanf("%d", &number[index]);
    }
    
    for (index = 0; index < testCases; index++) {
    	int j = 0;
    	for (j=1; j <= number[index]; j++) {
    		if (j%3 == 0)
    			printf("Fizz");
    		if (j%5 == 0)
    			printf("Buzz");
    		if (j%3 != 0 && j%5 != 0) {
    			printf("%d", j);
    		}
    		printf("\n");
    	}
    }
    return 0;
}
