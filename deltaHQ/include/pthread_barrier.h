/*
 * Copyright (c) 2019 kosay  All rights reserved.
 *
 * See the file 'LICENSE' for licensing information.
 *
 */
#ifdef __APPLE__

#ifndef PTHREAD_BARRIER_H_
#define PTHREAD_BARRIER_H_

#include <errno.h>
#include <pthread.h>
#include <stdint.h>

typedef int pthread_barrierattr_t;

typedef struct {
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    volatile uint32_t flag;
    size_t count;
    size_t num;
} pthread_barrier_t;
int pthread_barrier_init(pthread_barrier_t *bar,
                         const int *attr,
                         unsigned int num);
int pthread_barrier_destroy(pthread_barrier_t *barrier);

int pthread_barrier_wait(pthread_barrier_t *bar);
#endif // PTHREAD_BARRIER_H_
#endif // __APPLE__
