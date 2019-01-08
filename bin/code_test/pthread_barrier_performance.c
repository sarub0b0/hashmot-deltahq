#include <stdio.h>
#include <pthread.h>
#include <time.h>

#define PTHREAD_NUM 2

pthread_barrier_t br;
pthread_barrierattr_t br_attr;

pthread_barrier_init(&br, &br_attr, PTHREAD_NUM + 1);

void *brthread(void) {

    pthread_barrier_wait(&br);

    pthread_exit(NULL);
}

int main(void) {

    pthread_t pt[PTHREAD_NUM];

    for (int i = 0; i < PTHREAD_NUM; i++) {
        pt[i] = pthread_self();
        pthread_create(&pt[i], NULL, brthread, NULL);
    }
    pthread_barrier_wait(&br);

    for (int i = 0; i < PTHREAD_NUM; i++) {
        pthread_join(pt[i], NULL);
    }

    return 0;
}
