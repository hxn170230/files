#include <stdio.h>
#include <string.h>

int main() {
	int numTests = 0;
	scanf("%d", &numTests);

	int i = 0;
	for (i = 0; i < numTests; i++) {
		char s[1002] = {'\0',};
		char r[1002] = {'\0',};
		scanf("%s", s);
		if (s[strlen(s)-1] == '\n') {
			s[strlen(s)-1] = '\0';
		}
		int j = 0;
		int len = strlen(s);
		for (j = 1; j < len; j++) {
			if (s[0] <= s[j]) {
				char c = s[j];
				int k = j;
				while (k > 0) {
					s[k] = s[k-1];
					k--;
				}
				s[0] = c;
			}
		}
		printf("Case #%d: %s\n", i+1, s);
	}
}
