#ifndef INPUT_H
#define INPUT_H

#include <stdio.h>
#include <pthread.h>

typedef struct input_buffer {
    char **buffers;
    int write_pos;
    int read_pos;
    int bufs_size;
    int buf_size;
    int buf_count;
    pthread_mutex_t *mutex_value;
    pthread_mutex_t *mutex_queue;
    pthread_cond_t *cond;

    // FILE *test_json;
} input_buffer_t;

void *thread_buffer_write(void *arg);
char *buffer_read();

#endif
