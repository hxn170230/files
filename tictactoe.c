#include <stdio.h>
#include <time.h>
#include <limits.h>

void print(char array[][3]) {
	int i = 0;
	int j = 0;

	for (i = 0; i < 3; i++) {
		for (j = 0; j < 3; j++) {
			printf("%c ", array[i][j]);
		}
		printf("\n");
	}
}

int getRandom() {
	return rand()%3;
}

int win(char array[][3], char c) {
	int i = 0;
	int j = 0;
	int found = 0;
	char value = c;
	for (i = 0; i < 3; i++) {
		while (j < 3) {
			found += (value == array[i][j]);
			j++;
		}
		//printf("(%d)found = %d\n", i, found);
		if (found == 3) {
			return 1;
		} else {
			found = 0;
			j = 0;
		}
	}
	found = 0;
	j = 0;
	for (i = 0; i < 3; i++) {
		while (j < 3) {
			found += (value == array[j][i]);
			j++;
		}
		//printf("(%d)found = %d\n", i, found);
		if (found == 3) {
			return 1;
		} else {
			found = 0;
			j=0;
		}
	}
	found = 0;
	j = 0;
	for (i = 0; i < 3; i++) {
		found += (value==array[i][i]);
	}
		//printf("diag: found = %d\n", found);
	if (found == 3) {
		return 1;
	}
	found = 0;
	j = 2;
	for (i = 0; i < 3; i++) {
		found += (value==array[i][j--]);
	}
		//printf("(reverse diag)char %c found = %d\n", c, found);
	if (found == 3) {
		return 1;
	}
	return 0;
}

void ticTacToe(char a[][3], int turn, int *wins) {
	int i = getRandom();
	int j = getRandom();
	char c = (turn % 2 == 0)? 'X': 'O';
	int k = turn+1;
	int l = 0;
	char array[3][3];
	char m[10];
	for (i = 0; i < 3; i++) {
		for (j = 0; j < 3; j++) {
			array[i][j] = a[i][j];	
		}
	}

	int total_win = 0;
	for (i = 0; i < 3; i++) {
		for (j = 0; j < 3; j++) {
			if (array[i][j] != 95) {
				continue;
			} else {
				array[i][j] = c;
				if (!win(array, c)) {
					ticTacToe(array, k, wins);
				} else {
					wins[turn%2] += 1;
					return;
				}
				array[i][j] = 95;
			}
		}
	}
}

void ticTacToeGame(char array[][3], int turn) {
	int i = 0;
	int j = 0;
	int wins[2] = {0,0};
	int Owins = -INT_MAX;
	int maxi = 0;
	int maxj = 0;
	while (1) {
	if (turn % 2 == 0) {
		scanf("%d %d", &i, &j);
		array[i][j] = 'X';
		print(array);
		if (win(array, 'X')) {
			printf("YOU WIN\n");
			return;
		}
		turn++;
	} else {
		for (i = 0; i < 3; i++) {
			for (j = 0; j < 3; j++) {
				if (array[i][j] != 95) continue;
				array[i][j] = 'O';
				if (win(array, 'O')) {
					printf("I WIN\n");
					return;
				}
				ticTacToe(array, turn+1, wins);
				printf("X wins = %d\n", wins[0]);
				printf("O wins = %d\n", wins[1]);
				printf("minimax = %d\n", wins[1]-wins[0]);
				if ((wins[1]-wins[0]) > Owins) {
					maxi = i;
					maxj = j;
					Owins = wins[1]-wins[0];
				}
				wins[0] = 0;
				wins[1] = 0;
				array[i][j] = 95;
			}
		}
		array[maxi][maxj] = 'O';
		Owins = -INT_MAX;
		maxi = 0;
		maxj = 0;
		print(array);
		turn++;
	}
	}
}

int main() {
	char array[3][3];
	int i = 0;
	int j = 0;
	int wins[2] = {0,};
	for (i = 0; i < 3; i++) {
		for (j = 0; j < 3; j++) {
			array[i][j] = 95;
		}
	}
	ticTacToeGame(array, 0);
	return 0;
}
