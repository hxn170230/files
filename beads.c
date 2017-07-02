#include <stdio.h>

int findMaxCut(int n, char *str) {
	int i = 0;
	int max = 0;
	int b = 0;
	int r = 0;
	int w = 0;
	int startMax = -1;
	int prevr = -1;
	int prevb = -1;

	for (i = 0; i < n; i++) {
		if (str[i] == 'b') {
			b++;
		} else if (str[i] == 'r') {
			r++;
		}

		if (str[i] != str[(i+1)%n]) {
			if (str[i] == 'b') {
				prevb = b;
				r = 0;
				if (startMax == -1) {
					startMax = prevb;
				}
			} else if (str[i] == 'r') {
				prevr = r;
				b = 0;
				if (startMax == -1) {
					startMax = prevr;
				}
			}

			if (prevb > 0 && prevr > 0) {
				if (max < prevb + prevr) {
					max = prevb+prevr;
				} else {
					prevb = -1;
					prevr = -1;
				}
			}
		}
	}

	return max;
}

int main() {
	int n = 0;
	scanf("%d", &n);
	char str[n];
	scanf("%s", str);

	int max  = findMaxCut(n, str);
	printf("max: %d\n", max);
	return 0;
}
