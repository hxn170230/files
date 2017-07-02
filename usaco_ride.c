#include <stdio.h>
#include <string.h>

int main() {
	char array[1000] = {'\0',};
	scanf("%s", array);

	char array2[1000] = {'\0',};
	scanf("%s", array2);

	int len1 = strlen(array);
	int len2 = strlen(array2);
	int i = 0;

	int value = 1;
	int value2 = 1;
	for (i = 0; i < len1; i++) {
		value *= ((int)(array[i]-'A')+1);
	}

	for (i = 0; i < len2; i++) {
		value2 *= ((int)(array2[i]-'A')+1);
	}


	if (value%47 == value2%47) {
		printf("GO\n");
	} else {
		printf("STAY\n");
	}
	return 1;
}
