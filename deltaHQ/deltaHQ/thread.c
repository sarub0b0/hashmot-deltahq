#include "message.h"
#include "thread.h"

int thread_get_cpu_number(void) {
    int num = 0;
#ifndef __APPLE__
    cpu_set_t cpu;
    CPU_ZERO(&cpu);
    if (pthread_getaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpu) !=
        0) {
        WARNING("sched_getaffinity error");
        return ERROR;
    }

    num = CPU_COUNT(&cpu);
#endif

    return num;
}

int thread_get_index(struct array_range *ar) {
    ar->end = ar->loop;
    ar->loop -= ar->step;
    if (0 < ar->mod) {
        ar->loop--;
    }
    ar->mod--;
    ar->start = ar->loop;

    return SUCCESS;
}
