#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main() {
	float estdRtt = 4;
	float sampleRtt = 1;
	int i = 0;
	float diff = 0;
	float dev = 1;
	float timeout = 10;
	float delta = (float)1/8;

		while (timeout >= 4) {
			diff = sampleRtt - estdRtt;
			estdRtt = estdRtt + delta*(diff);
			dev = dev + delta*(fabsf(diff) - dev);
			timeout = 1*estdRtt + 4*dev;
			i++;
			printf("Iteration: %d Timeout: %f\n", i, timeout);
	}
	return 0;
}
