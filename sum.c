#include<stdio.h>

int main() {
	int testCases = 0;
	scanf("%d", &testCases);
	int i = 0;
	for (i = 0; i < testCases; i++) {
		int a = 0;
		int b = 0;
		scanf("%d %d", &a, &b);
		printf("%d", a+b);
	}
	return 0;
}
