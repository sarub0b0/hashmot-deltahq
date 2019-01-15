
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>

struct msg {
    int8_t type;
    int32_t id;
    float x;
    float y;
    int16_t neighbor_size;
    int32_t neighbor[1];
};
int main(void) {
    struct sockaddr_in si_me;
    int s;
    int port      = 10000;
    int broadcast = 1;

    s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    setsockopt(s, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof broadcast);

    memset(&si_me, 0, sizeof(si_me));
    si_me.sin_family      = AF_INET;
    si_me.sin_port        = htons(port);
    si_me.sin_addr.s_addr = INADDR_ANY;

    // assert(:: != -1);
    bind(s, (struct sockaddr *) &si_me, sizeof(struct sockaddr));

    struct msg *msg = malloc(9000);

    int recv_len = 0;
    while (1) {
        // char buf[9000] = {0};
        recv_len = recv(s, msg, 9000, 0);
        if (recv_len < 1) {
            printf("error\n");
            exit(1);
        }
        // memcpy(msg, buf, recv_len);

        if (msg->type == -1) {
            printf("finish\n");
            continue;
            // exit(0);
        }
        printf("%d: %d(%.2f, %.2f) neighbor(%d): ",
               msg->type,
               msg->id,
               msg->x,
               msg->y,
               msg->neighbor_size);
        // for (int32_t i = 0; i < msg->neighbor_size; i++) {
        //     // for (int16_t i = 0; i < 10; i++) {
        //     printf("%d ", msg->neighbor[i]);
        // }
        puts("");
    }
}
