#include <stdio.h>
#include <string.h>

int firstNegIndex(char *s, int k) {
	while(s[k] != '-') {
		k++;
	}
	return k;
}

int lastContinuousNegIndex(char *s, int j) {
	while(s[j] == '-') {
		j++;
	}
	return j-1;
}

int countSteps(char *s) {
	int i = -1;
	int j = -1;
	int k = -1;
	int steps = 0;
	int length = strlen(s);
	while (k < length) {
		j = firstNegIndex(s, k+1);
		if (j < 0 || j > strlen(s)) {
			break;
		}
		i = j - 1;
		k = lastContinuousNegIndex(s, j);
		if (k < 0 || k > strlen(s)) {
			break;
		}
		while (j <= k) {
			s[j] = '+';
			j++;
		}
		if (i < 0) {
			steps+=1;
		} else {
			steps+=2;
		}
		k+=1;
	}
	return steps;
}

int main() {
	int numTests = 0;
	scanf("%d", &numTests);

	int i = 0;
	for (i = 0; i < numTests; i++) {
		char s[200] = {'\0',};
		scanf("%s", s);
		if (s[strlen(s)-1] == '\n') {
			s[strlen(s)-1] = '\0';
		}

		int count = countSteps(s);
		printf("Case #%d: %d\n", i+1, count);
	}
	return 1;
}
