#ifndef SOCKET_H
#define SOCKET_H

#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>

#include <string>

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

class DGram {
   public:
    DGram();
    ~DGram();

    void Open(const std::string &type);

    bool Bind(const std::string &host, const std::string &port);

    ssize_t Sendto(const void *buf, size_t length, int flag);
    ssize_t Recvfrom(void *buf, size_t length);

   private:
    int fd_;
    int family_;

    char buf_[BUFSIZ];

    struct sockaddr_storage to_addr_;
    struct sockaddr_storage from_addr_;
};

#endif
