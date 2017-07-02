#include <stdio.h>

int main() {
	float cost = 0;
	scanf("%09f", &cost);

	float totalCost = 0;
	int numLawns = 0;
	scanf("%d", &numLawns);
	int i = 0;
	for (i = 0; i < numLawns; i++) {
		float width = 0;
		scanf("%09f", &width);
		float length = 0;
		scanf("%09f", &length);

		totalCost = totalCost + (width * length * cost);
	}

	printf("%09f\n", totalCost);
}
