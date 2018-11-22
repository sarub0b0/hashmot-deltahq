#include <array>
#include <vector>
#include <algorithm>

using namespace std;

int main(void) {

    vector<array<int, 2>> point{
        {1, 1}, {1, 2}, {1, 3}, {1, 4}, {1, 5},
        {1, 6}, {1, 7}, {1, 8}, {1, 9}, {1, 10},

        {2, 1}, {2, 2}, {2, 3}, {2, 4}, {2, 5},
        {2, 6}, {2, 7}, {2, 8}, {2, 9}, {2, 10},
    };

    printf("default\t");
    for (auto &&p : point) {
        printf("(%d, %d)\n", p[0], p[1]);
    }
    puts("");

    printf("sort v1\t");
    int axis = 1;

    sort(point.begin(),
         point.end(),
         [&](const array<int, 2> &x, const array<int, 2> &y) {
             return x[axis] < y[axis];
         });

    for (auto &&p : point) {
        printf("(%d, %d)\n ", p[0], p[1]);
    }
    puts("");

    printf("sort v2\t");
    axis = 0;
    sort(point.begin(),
         point.end(),
         [&](const array<int, 2> &x, const array<int, 2> &y) {
             return x[axis] < y[axis];
         });
    for (auto &&p : point) {
        printf("(%d, %d)\n ", p[0], p[1]);
    }
    puts("");
    return 0;
}
