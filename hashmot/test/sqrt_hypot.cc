#include <chrono>
#include <cmath>
#include <cstdio>

using namespace std;
using namespace std::chrono;

#define loop 100000
int main(void) {

    high_resolution_clock::time_point begin, end;
    nanoseconds elapsed;

    float d;

    begin = high_resolution_clock::now();
    for (int i = 0; i < loop; i++) {
        d = hypot(1 + i, 2 + i);
    }
    end     = high_resolution_clock::now();
    elapsed = duration_cast<nanoseconds>(end - begin);

    printf("hypot: %lld.%09lld\n",
           elapsed.count() / 1000000000,
           elapsed.count() % 1000000000);

    begin = high_resolution_clock::now();
    for (int i = 0; i < loop; i++) {
        d = sqrt(pow(1 + i, 2) + pow(2 + i, 2));
    }
    end     = high_resolution_clock::now();
    elapsed = duration_cast<nanoseconds>(end - begin);
    printf("sqrt:  %lld.%09lld\n",
           elapsed.count() / 1000000000,
           elapsed.count() % 1000000000);
}
