#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(void) {

    int *a = (int *) malloc(sizeof(int) * 100);

    printf("a  %lu\n", sizeof(a));
    printf("*a %lu\n", sizeof(*a));

    return 0;
}
