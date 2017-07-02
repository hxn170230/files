#include <stdio.h>

int fib(int n) {
	long long int a = 1;
	long long int b = 1;

	int i = 0;
	for (i = 3; i < n+1; i+=2) {
		a = a + b;
		b = a + b;
		printf("f[%d]: %lld f[%d]: %lld\n", i, a, i+1, b);
	}

	printf("a:%lld\n", a);
	return a;
}

int main() {
	fib(1000000000);
	return 0;
}
