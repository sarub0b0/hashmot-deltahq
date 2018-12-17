#include <stdio.h>

#include "../include/global.h"

int main(void) {
    int loop = 0;

    scanf("%d", &loop);

    int a = 0;

    TCHK_START(a);
    for (int i = 0; i < loop; i++) {
        a++;
    }
    TCHK_END(a);

    a = 0;
    TCHK_START(b);
    for (int i = 0; i < loop; i++) {
        ++a;
    }
    TCHK_END(b);

    a = 0;
    TCHK_START(c);
    for (int i = 0; i < loop; ++i) {
        a++;
    }
    TCHK_END(c);

    a = 0;
    TCHK_START(d);
    for (int i = 0; i < loop; ++i) {
        ++a;
    }
    TCHK_END(d);

    printf("%d\n", a);
}
