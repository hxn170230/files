#include <stdio.h>

void print_spiral(int m, int n, int array[][n]) {
    int i = 0, j = 0, k = 0, l = 0;
        while ((n-(k+1) > k) && (m-(l+1) > l)) {
	i = l;
        for (j = k; j < n-(k); j++) {
            printf("%d ", array[i][j]);
        }
	j = n-(k+1);
        for (i = l+1; i < m-(l+1); i++) {
            printf("%d ", array[i][j]);
        }
	i = m-(l+1);
        for (j = n-(k+1); j >= k; j--) {
            printf("%d ", array[i][j]);
        }
	j = k;
        for (i = m-(l+2); i >= l+1; i--) {
            printf("%d ", array[i][j]);
        }
        k++;
        l++;
    }
}

int main() {
 int numtests = 0;
 scanf("%d", &numtests);

 int i = 0;
 for (i = 0; i < numtests; i++) {
   int m = 5;
   int n = 6;
   int j = 0;
   int k = 0;
   int array[m][n];
   int l = 1;
   for (j = 0; j < m; j++) {
     for (k = 0; k < n; k++) {
       //scanf("%d", &array[j][k]);
       array[j][k] = l++;
     }
   }
   print_spiral(j, k, array);
 }
 return 0;
}
