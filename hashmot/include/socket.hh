// MIT License

// Copyright (c) 2019 kosay

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
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
