#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include <message.h>
#include <input.h>
#include <json.h>

void *thread_buffer_write(void *arg) {
    input_buffer_t *ibuf = (input_buffer_t *) arg;
    int is_finish        = 0;

    while (!is_finish) {
        if (pthread_mutex_lock(ibuf->mutex_queue) != 0) {
            WARNING("pthread_mutex_lock error");
            return NULL;
        }

        while (ibuf->bufs_size <= ibuf->buf_count) {
            if (pthread_cond_wait(ibuf->cond, ibuf->mutex_queue) != 0) {

                WARNING("pthread_cond_wait error");
                return NULL;
            }
        }

        if (pthread_cond_broadcast(ibuf->cond) != 0) {
            WARNING("pthread_cond_broadcast error");
            return NULL;
        }

        if (pthread_mutex_unlock(ibuf->mutex_queue) != 0) {
            WARNING("pthread_mutex_unlock error");
            return NULL;
        }

        while (fgets(ibuf->buffers[ibuf->write_pos], ibuf->buf_size, stdin) ==
               NULL) {
        }

        if (strncmp(ibuf->buffers[ibuf->write_pos],
                    "{\"finish\":\"finish\"}\n",
                    20) == 0) {
            is_finish = 1;
        }

        if (pthread_mutex_lock(ibuf->mutex_value) != 0) {
            WARNING("pthread_mutex_lock error");
            return NULL;
        }

        ibuf->buffers[ibuf->write_pos]
                     [strlen(ibuf->buffers[ibuf->write_pos])] = '\0';
        ibuf->write_pos = (ibuf->write_pos + 1) % ibuf->bufs_size;
        ibuf->buf_count++;
        if (pthread_mutex_unlock(ibuf->mutex_value) != 0) {
            WARNING("pthread_mutex_unlock error");
            return NULL;
        }
    }

    fprintf(stdout, "pthread_exit thread_buffer_write\n");
    pthread_exit(NULL);
    pthread_detach(pthread_self());
    return NULL;
}

char *buffer_read(input_buffer_t *ibuf) {
    char *read;

    if (pthread_mutex_lock(ibuf->mutex_queue) != 0) {
        WARNING("pthread_mutex_lock error");
        return NULL;
    }

    while (ibuf->buf_count <= 0) {
        if (pthread_cond_wait(ibuf->cond, ibuf->mutex_queue) != 0) {
            WARNING("pthread_cond_wait error");
            return NULL;
        }
    }

    if (pthread_cond_broadcast(ibuf->cond) != 0) {
        WARNING("pthread_cond_broadcast error");
        return NULL;
    }
    if (pthread_mutex_unlock(ibuf->mutex_queue) != 0) {
        WARNING("pthread_mutex_unlock error");
        return NULL;
    }

    if (pthread_mutex_lock(ibuf->mutex_value) != 0) {
        WARNING("pthread_mutex_lock error");
        return NULL;
    }

    read           = ibuf->buffers[ibuf->read_pos];
    ibuf->read_pos = (ibuf->read_pos + 1) % ibuf->bufs_size;
    ibuf->buf_count--;

    if (pthread_mutex_unlock(ibuf->mutex_value) != 0) {
        WARNING("pthread_mutex_unlock error");
        return NULL;
    }

    return read;
}
