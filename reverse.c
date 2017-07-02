#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
int main ()
{
    int x = 11;
    double reverse = 0;
    int sign = 0;
    int index = 0;
    
    if (x < 0) return 0;
    if (x == 0) return 1;
    
    if (x/10 == 0) return 1;
    
    while (x != 0) {
        reverse = reverse * 10 + x%10;
        x = x/10;
    }
    
    printf("%G\n", reverse);
    if (reverse >= INT_MAX) {
        return 0;
    }
    int number = (int)reverse;
    printf("%d %d\n", number, (number == x)); 
    return ((int)reverse == x);
}
