#include <stdio.h>

void mintomax(int *array, int size) {
 int i = size-1;
 for (i = size-1; i > 0; i--) {
  int child = array[i];
  int parent = array[(i-1)/2];
  if (parent < child) {
   array[i] = parent;
   array[(i-1)/2] = child;
  }
 }
}

int main() {
 int size = 0;
 scanf("%d", &size);
 int j = 0;
 int array[size];
 for (j = 0; j < size; j++) {
  scanf("%d", &array[j]);
 }
 mintomax(array, size);
 for (j = 0; j < size; j++) {
  printf("%d ", array[j]);
 }
 printf("\n");
 return 0;
}
