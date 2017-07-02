#include <stdio.h>

int main() {
	int a[2] = {0,};
	int b[2] = {0,};
	int c[2] = {0,};
	int mid[2] = {0,};

	scanf("%d %d", &a[0], &a[1]);
	scanf("%d %d", &b[0], &b[1]);
	scanf("%d %d", &c[0], &c[1]);

	// find correct order of indices
	float distance1 = (a[0] - b[0])*(a[0]-b[0]) + (a[1] - b[1])*(a[1]-b[1]);
	float distance2 = (a[0] - c[0])*(a[0]-c[0]) + (a[1] - c[1])*(a[1]-c[1]);
	float distance3 = (b[0]-c[0])*(b[0]-c[0]) + (b[1]-c[1])*(b[1]-c[1]);
	if (distance1 == distance2) {
		mid[0] = a[0];
		mid[1] = a[1];
		a[0] = b[0];
		a[1] = b[1];
	} else if (distance3 == distance1) {
		mid[0] = b[0];
		mid[1] = b[1];
	} else if (distance2 == distance3) {
		mid[0] = c[0];
		mid[1] = c[1];
		c[0] = b[0];
		c[1] = b[1];
	} else {
		//square not possible
		//printf("Square not possible\n");
		return -1;
	}

	float k2 = ((c[0]*c[0] + c[1]*c[1]) - (a[0]*a[0] + a[1]*a[1]))/2;
	float sideLength = (mid[0]-a[0]) * (mid[0]-a[0]) + (mid[1]-a[1]) * (mid[1]-a[1]);
	float diagLength = (c[0]-a[0])*(c[0]-a[0]) + (c[1]-a[1])*(c[1]-a[1]);
	float d1 = 0;
	float d2 = 0;

	float x = c[0]-a[0];
	float y = c[1]-a[1];
	//d1 = (k2 - yd2)/x
	float i = 0;
	for (i = -10000; i <= 10000; i++) {
		if (x != 0) {
			d2 = i;
			d1 = (k2-(y*d2))/x;
		} else {
			d1 = i;
			d2 = (k2-(x*d1))/y;
		}
		// verify if d1 and d2 befloat to square
		float firstlength = (d1-a[0])*(d1-a[0]) + (d2-a[1])*(d2-a[1]);
		float secondlength = (d1-c[0])*(d1-c[0]) + (d2-c[1])*(d2-c[1]);
		if ((firstlength == sideLength) && (secondlength == sideLength)) {
			float diaglength = (d1-mid[0])*(d1-mid[0]) + (d2-mid[1])*(d2-mid[1]);
			if (diagLength == diaglength) {
				printf("%0.0f %0.0f\n", d1, d2);
			}
		}
	}
	return 0;
}
