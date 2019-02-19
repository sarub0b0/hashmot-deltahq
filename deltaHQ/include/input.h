/*
 * Copyright (c) 2019 kosay  All rights reserved.
 *
 * See the file 'LICENSE' for licensing information.
 *
 */
#ifndef INPUT_H
#define INPUT_H

#include <stdio.h>
#include <pthread.h>

#include "socket.h"

typedef struct input_buffer {
    char **buffers;
    int write_pos;
    int read_pos;
    int bufs_size;
    long buf_size;
    int buf_count;
    int is_listen_dgram;

    dgram_info_t dgram;

    // int is_writable;
    // int read_idx;
    // int write_idx;
    pthread_mutex_t *mutex_value;
    pthread_mutex_t *mutex_queue;
    pthread_cond_t *cond;
    // pthread_mutex_t *mutex_write_buffer;
    // pthread_cond_t *cond_write_buffer;

    // FILE *test_json;
} input_buffer_t;

void *thread_buffer_write(void *arg);
char *buffer_read();
void free_read_string();
int allowed_write_buffer();

#endif
