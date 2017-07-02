#include <stdio.h>
#include <string.h>

int flip(char *s, int j, int k, int len) {
	if (j+k > len) {
		return -1;
	} else {
		int l = 0;
		while (l < k) {
			if (s[j+l] == '-')
				s[j+l] = '+';
			else
				s[j+l] = '-';
			l++;
		}
		return 0;
	}
}

int main() {
	int t = 0;
	int i = 0;
	int j = 0;
	scanf("%d", &t);

	for (i = 0; i < t; i++) {
		int k = 0;
		int n = 0;
		int impossible = 0;
		char s[1000] = {'\0',};

		scanf("%s %d", s, &k);
		int len = strlen(s);

		for (j = 0; j < len; j++) {
			if (s[j] == '-') {
				int f = flip(s, j, k, len);
				if (f < 0) {
					impossible = 1;
					break;
				} else {
					n++;
				}
			}
		}
		if (impossible == 0) {
			printf("Case #%d: %d\n", i+1, n);
		} else {
			printf("Case #%d: IMPOSSIBLE\n", i+1);
		}
	}
	return 0;
}
