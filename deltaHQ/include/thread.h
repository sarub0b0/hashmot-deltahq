#ifndef __THREAD_H
#define __THREAD_H

#include <pthread.h>

#include "deltaQ.h"

#ifdef __APPLE__
#include "pthread_barrier.h"
#endif

#define NAME_SIZE 16

#define MOTION_OUTPUT_NAM 0
#define MOTION_OUTPUT_NS2 1

// struct neighbour{
//     int start;
//     int end;
//     int center_node_id;
// };

struct array_range {
    int start;
    int end;
    int loop;
    int step;
    int mod;
};

struct thread_connection_class {
    struct scenario_class *scenario;
    int start;
    int end;
};

struct scenario_deltaQ_class {
    struct scenario_class *scenario;
    struct connection_class **neighbor;
    int calc_done;
    int *start_conn;
    int *end_conn;
    pthread_barrier_t *br_assign;
    pthread_barrier_t *br_calc;
    pthread_barrier_t *br_motion;
    pthread_mutex_t *mut_set_index;
};

int thread_get_cpu_number(void);

int thread_get_index(struct array_range *ar);

#endif
