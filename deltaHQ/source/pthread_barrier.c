/*
 * Copyright (c) 2019 kosay  All rights reserved.
 *
 * See the file 'LICENSE' for licensing information.
 *
 */
#ifdef __APPLE__
#include <pthread_barrier.h>

int pthread_barrier_destroy(pthread_barrier_t *barrier) {
    pthread_cond_destroy(&barrier->cond);
    pthread_mutex_destroy(&barrier->mutex);
    return 0;
}

int pthread_barrier_init(pthread_barrier_t *bar,
                         const int *attr,
                         unsigned int num) {
    if (num == 0) {
        errno = EINVAL;
        return -1;
    }
    if (pthread_mutex_init(&(bar->mutex), 0) < 0) {
        return -1;
    }
    if (pthread_cond_init(&(bar->cond), 0) < 0) {
        return -1;
    }
    bar->flag  = 0;
    bar->count = 0;
    bar->num   = num;
    return 0;
}

int pthread_barrier_wait(pthread_barrier_t *bar) {
    int ret       = 0;
    uint32_t flag = 0;

    if ((ret = pthread_mutex_lock(&(bar->mutex)))) {
        return ret;
    }

    flag = bar->flag;
    bar->count++;

    if (bar->count == bar->num) {
        bar->count = 0;
        bar->flag  = 1 - bar->flag;
        if ((ret = pthread_cond_broadcast(&(bar->cond)))) {
            return ret;
        }
        if ((ret = pthread_mutex_unlock(&(bar->mutex)))) {
            return ret;
        }
        return 1;
    }

    while (1) {
        if (bar->flag == flag) {
            ret = pthread_cond_wait(&(bar->cond), &(bar->mutex));
            if (ret) {
                return ret;
            }
        } else {
            break;
        }
    }

    if ((ret = pthread_mutex_unlock(&(bar->mutex)))) {
        return ret;
    }
    return 0;
}
#endif
