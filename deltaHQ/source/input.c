/*
 * Copyright (c) 2019 kosay  All rights reserved.
 *
 * See the file 'LICENSE' for licensing information.
 *
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include <global.h>
#include <message.h>
#include <input.h>
#include <json.h>

void *thread_buffer_write(void *arg) {
    input_buffer_t *ibuf = (input_buffer_t *) arg;
    int is_finish        = 0;
    int recv_len         = 0;

    while (1) {
        if (pthread_mutex_lock(ibuf->mutex_queue) != 0) {
            WARNING("pthread_mutex_lock error");
            return NULL;
        }

        while (ibuf->bufs_size - 1 <= ibuf->buf_count) {
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

        if (is_finish) {
            goto FINAL_HANDLE;
        }

        // if (pthread_mutex_lock(ibuf->mutex_write_buffer) != 0) {
        //     WARNING("pthread_mutex_lock error");
        //     return NULL;
        // }
        // while (ibuf->is_writable == 0) {
        //     if (pthread_cond_wait(ibuf->cond_write_buffer,
        //                           ibuf->mutex_write_buffer)) {
        //         WARNING("pthread_cond_wait error");
        //         return NULL;
        //     }
        // }
        // if (pthread_mutex_unlock(ibuf->mutex_write_buffer) != 0) {
        //     WARNING("pthread_mutex_unlock error");
        //     return NULL;
        // }
        if (ibuf->is_listen_dgram) {
            recv_len = recv(ibuf->dgram.sock,
                            ibuf->buffers[ibuf->write_pos],
                            ibuf->buf_size,
                            0);
            if (recv_len == 0) {
                return NULL;
            }
            if (recv_len == -1) {
                perror("recv");
                return NULL;
            }
            ibuf->buffers[ibuf->write_pos][recv_len] = '\0';
            // printf("%s\n", ibuf->buffers[ibuf->write_pos]);
        } else {
            while (fgets(ibuf->buffers[ibuf->write_pos],
                         ibuf->buf_size,
                         stdin) == NULL) {
            }
            ibuf->buffers[ibuf->write_pos]
                         [strlen(ibuf->buffers[ibuf->write_pos])] = '\0';
        }

        if (strncmp(ibuf->buffers[ibuf->write_pos],
                    "{\"finish\":\"finish\"}",
                    19) == 0) {
            is_finish = 1;
        }

        if (pthread_mutex_lock(ibuf->mutex_value) != 0) {
            WARNING("pthread_mutex_lock error");
            return NULL;
        }

        ibuf->write_pos = (ibuf->write_pos + 1) % ibuf->bufs_size;
        ibuf->buf_count++;

        // ibuf->write_idx = (ibuf->write_idx + 1) % ibuf->bufs_size;
        // printf("read=%d write=%d count=%d\n",
        //        ibuf->read_idx,
        //        ibuf->write_idx,
        //        ibuf->buf_count);

        if (pthread_mutex_unlock(ibuf->mutex_value) != 0) {
            WARNING("pthread_mutex_unlock error");
            return NULL;
        }
    }

FINAL_HANDLE:

#ifdef DEBUG_PRINT
    fprintf(stdout, "pthread_exit thread_buffer_write\n");
#endif
    pthread_exit(NULL);
    // pthread_detach(pthread_self());
    return NULL;
}

// char *global_read = NULL;
char *buffer_read(input_buffer_t *ibuf) {
    char *read = NULL;
    // int read_len = 0;

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
    read = ibuf->buffers[ibuf->read_pos];

    // printf("%p %s", read, read);
    // read_len    = strlen(read) + 1;
    // global_read = (char *) malloc(sizeof(char) * read_len);
    // memcpy(global_read, read, read_len);
    // printf("%p %s", global_read, global_read);

    ibuf->read_pos = (ibuf->read_pos + 1) % ibuf->bufs_size;
    ibuf->buf_count--;
    // ibuf->read_idx = (ibuf->read_idx + 1) % ibuf->bufs_size;

    // printf("read=%d write=%d count=%d\n",
    //        ibuf->read_idx,
    //        ibuf->write_idx,
    //        ibuf->buf_count);

    // if (pthread_mutex_lock(ibuf->mutex_write_buffer) != 0) {
    //     WARNING("pthread_mutex_lock error");
    //     return NULL;
    // }

    // if (ibuf->bufs_size - 1 <= ibuf->buf_count) {
    //     printf("not writtable");
    //     ibuf->is_writable = 0;
    // }

    // if (pthread_cond_broadcast(ibuf->cond_write_buffer) != 0) {
    //     WARNING("pthread_cond_broadcast");
    //     return NULL;
    // }
    // if (pthread_mutex_unlock(ibuf->mutex_write_buffer) != 0) {
    //     WARNING("pthread_mutex_unlock error");
    //     return NULL;
    // }
    if (pthread_mutex_unlock(ibuf->mutex_value) != 0) {
        WARNING("pthread_mutex_unlock error");
        return NULL;
    }

    return read;
    // return global_read;
}

// int allowed_write_buffer(input_buffer_t *ibuf) {
//     if (pthread_mutex_lock(ibuf->mutex_write_buffer) != 0) {
//         WARNING("pthread_mutex_lock error");
//         return ERROR;
//     }

//     printf("writtable");
//     ibuf->is_writable = 1;

//     if (pthread_cond_broadcast(ibuf->cond_write_buffer) != 0) {
//         WARNING("pthread_cond_broadcast");
//         return ERROR;
//     }
//     if (pthread_mutex_unlock(ibuf->mutex_write_buffer) != 0) {
//         WARNING("pthread_mutex_unlock error");
//         return ERROR;
//     }

//     return SUCCESS;
// }

// void free_read_string() {
//     if (global_read) {
//         free(global_read);
//         global_read = NULL;
//     }
// }
