#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main() {
	float lambda = 1;
	int i = 0;
	float time = 0;

	for (; lambda < 8.1; lambda+=0.1) {
		for (i = 0; i < 1000; i++) {
			float futt = 0;
			float prevt = -1;
			while (futt == 0) {
				futt = (float)(rand()%RAND_MAX)/(float)(RAND_MAX);
			}
			float t = (-lambda)*log(futt);
			float x = t + (-lambda)*log(futt);
			while ((t-prevt < 1) || (x-t < 1)) {
				prevt = t;
				t = x;
				futt = 0;
				while (futt == 0) {
					futt = (float)(rand()%RAND_MAX)/(float)(RAND_MAX);
				}
				x += (-lambda)*log(futt);
			}
			time += t;
		}
		printf("lambda: %0.02f avg slots: %0.02f\n", lambda, time/1000);
		printf("useful slot percentage: %0.02f\n", 8/(time/1000 + 8));
		time = 0;
	}
	return 0;
}
