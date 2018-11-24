#include <iostream>

int main(void) {

    int axis = 0;

    axis = axis == 0 ? 1 : 0;
    printf("%d\n", axis);

    axis = axis == 0 ? 1 : 0;
    printf("%d\n", axis);

    float a, b;
    a = 0.1;
    b = 0.1;
    printf("a=%f \nb=%f\n", a, b);
    if (a < b) {
        printf("equal\n");
    } else {
        printf("b big\n");
    }

    return 0;
}
