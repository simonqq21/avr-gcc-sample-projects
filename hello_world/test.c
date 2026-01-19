#include <stdio.h>

#define PD3 3

int led1 = PD3;

int main()
{
    printf("PD3 = %d\n", 1 << PD3);
    printf("led1 = %d\n", 1 << led1);
    printf("!led1 = %d\n", !(1 << led1));
    printf("~led1 = %d\n", ~(1 << led1));
    return 0;
}