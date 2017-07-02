#include <stdio.h>
#include <math.h>

int main() {
	int numTests = 0;
	scanf("%d", &numTests);

	int i =0;
	for (i =0; i<numTests; i++) {
		int numVertices = 0;
		scanf("%d", &numVertices);
		int j = 0;
		int vertices[numVertices][2];
		for (j = 0; j < numVertices; j++) {
			scanf("%d %d", &vertices[j][0], &vertices[j][1]);
		}
		double area = 0;
		for (j = 0; j < numVertices; j++) {
			area += (vertices[j][0]*vertices[(j+1)%numVertices][1]) - (vertices[j][1]*vertices[(j+1)%numVertices][0]);
		}
		printf("%0.1f\n", area/2);
	}
	return 0;
}
