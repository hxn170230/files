#include <stdio.h>

int foundAllDigits(int *digits) {
	int i = 0;
	for (i = 0; i < 10; i++) {
		if (digits[i] <= 0) {
			return 0;
		}
	}
	return 1;
}

int main() {
	int numTests = 0;
	scanf("%d", &numTests);

	int i = 0;
	for (i = 0; i < numTests; i++) {
		int digits[10] = {0, };
		long long number = 0;
		scanf("%lld", &number);
		if (number == 0) {
			printf("Case #%d: INSOMNIA\n", i+1);
			continue;
		} else {
			int done = 0;
			int count = 2;
			long long local = number;
			while (!done) {
				while (local != 0) {
					digits[local%10] = 1;
					local = local/10;
				}
				if (foundAllDigits(digits)) {
					printf("Case #%d: %lld\n", i+1, number*(count-1));
					done = 1;
				} else {
					local = number * count;
					count++;
				}
			}
		}
	}
}
