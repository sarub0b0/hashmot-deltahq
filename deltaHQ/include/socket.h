#ifndef SOCKET_H
#define SOCKET_H

#include <arpa/inet.h>

typedef struct broadcast_info bc_info_t;
/*
 *  ブロードキャスト情報
 */
struct broadcast_info {
    unsigned short port;   /* ポート番号 */
    char *ipaddr;          /* IPアドレス */
    char *msg;             /* 送信メッセージ */
    unsigned long msg_len; /* メッセージ長 */

    int sd;                  /* ソケットディスクリプタ */
    struct sockaddr_in addr; /* ブロードキャストのアドレス構造体 */
    int permission;          /* ブロードキャストの許可設定 */
};

int socket_init_state(char *ipaddr, unsigned short port, bc_info_t *info);
// int broadcast_sender(bc_info_t *info);

void socket_finalize(bc_info_t *info);

int broadcast_sendmsg(bc_info_t *info);

struct dgram_info {
    unsigned short port;
    int sock;
    struct sockaddr_in addr;
} typedef dgram_info_t;

int dgram_listen(struct dgram_info *dg_info);
#endif
