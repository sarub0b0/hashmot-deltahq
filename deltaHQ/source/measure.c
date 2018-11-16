#include "measure.h"
#include <stdio.h>
#include <sys/syscall.h>
#include <unistd.h>

int get_cpu(int *cpu) {

    char statpath[256];
    sprintf(statpath, "/proc/%ld/stat", syscall(SYS_gettid));
    FILE *fp = fopen(statpath, "r");
    if (fp != NULL) {
        // No.       1   2   3   4   5   6   7   8   9   10  11  12  13  14  15
        // 16  17  18  19  20  21  22  23  24  25  26  27  28  29  30  31  32
        // 33  34  35  36  37  38  39
        fscanf(fp,
               "%*d %*s %*c %*d %*d %*d %*d %*d %*u %*u %*u %*u %*u %*u %*u "
               "%*d %*d %*d %*d %*d %*d %*u %*u %*d %*u %*u %*u %*u %*u %*u "
               "%*u %*u %*u %*u %*u %*u %*u %*d %d",
               cpu);
    }
    fclose(fp);

    return 0;
}
