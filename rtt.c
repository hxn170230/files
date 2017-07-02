#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main() {
	float estdRtt = 4;
	float sampleRtt = 1;
	int i = 0;
	int j = 2;
	float diff = 0;
	float dev = 1;
	float timeout = 10;
	float delta = (float)1/8;
	float nSampleRtt = 4;
	int N = 0;

	while (j <= 20) {
		i = 0;
		timeout = 10;
		estdRtt = 4;
		sampleRtt = 1;
		diff = 0;
		dev = 1;
		int k = 1;
		while (timeout >= 4 && i <= 1000) {
			if (j == k && k != 0) {
				diff = nSampleRtt - estdRtt;
				k = 1;
			} else {
				diff = sampleRtt - estdRtt;
				k++;
			}
			estdRtt = estdRtt + delta*(diff);
			dev = dev + delta*(fabsf(diff) - dev);
			timeout = 1*estdRtt + 4*dev;
			i++;
		}
		j++;
		if (N < j && i >= 1000) N = j;
		printf("N = %d timeout: %f\n", N, timeout);
	}
	return 0;
}
