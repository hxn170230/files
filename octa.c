#include <stdio.h>
#include <stdlib.h>

void findValues(int value, int *octa, int *hexa, int *deci) {
	if (value == 0) {
		*octa = 0;
		*hexa = 0;
		*deci = 0;
		return;
	} else {
		int reminder = value %10;
		findValues(value/10, octa, hexa, deci);
		*octa = (*octa)*8 + reminder;
		*deci = (*deci)*10 + reminder;
		*hexa = (*hexa)*16 + reminder;
	}
}

void findval(int value, int *endValue, int base, int *noError) {
	if (value == 0) {
		*endValue = 0;
		*noError = 1;
		return;
	} else {
		int reminder = value % 10;
		if (reminder >= base) {
			*endValue = 0;
			*noError = 0;
			return;
		} else {
			findval(value/10, endValue, base, noError);
			if (*noError) {
				*endValue = base * (*endValue) + reminder;
			}
		}
	}
}

void findValue(int value, int *endValue, int base) {
	if (value == 0) {
		*endValue = 0;
		return;
	} else {
		int noError = 1;
		findval(value, endValue, base, &noError);
	}
}

int main() {
	int value = 0;
	scanf("%d", &value);
	int octaValue = 0;
	int hexaValue = 0;
	int decimalValue = 0;

	findValues(value, &octaValue, &hexaValue, &decimalValue);

	printf("%d %d %d\n", octaValue, decimalValue, hexaValue);
	octaValue = 0;
	decimalValue = 0;
	hexaValue = 0;
	findValue(value, &octaValue, 8);
	findValue(value, &decimalValue, 10);
	findValue(value, &hexaValue, 16);
	printf("%d %d %d\n", octaValue, decimalValue, hexaValue);
	return 1;
}
