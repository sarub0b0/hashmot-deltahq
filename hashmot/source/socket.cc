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



#include <dgram.hh>



// #include <cstdio>
// #include <cstdlib>
// #include <string>

// #include <sys/socket.h>
// #include <arpa/inet.h>
// #include <errno.h>

// #include <socket.hh>
/*!
 * @brief      ソケットの初期化
 * @param[in]  info   ブロードキャスト情報
 * @param[out] errmsg エラーメッセージ格納先
 * @return     成功ならば0、失敗ならば-1を返す。
 */
// static int socket_initialize(bc_info_t *info) {
//     int rc = 0;

//     [> ソケットの生成 : UDPを指定する <]
//     info->sd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
//     if (info->sd < 0) {
//         WARNING("socket");
//         return ERROR;
//     }

//     [> ブロードキャスト機能を有効にする <]
//     rc = setsockopt(info->sd,
//                     SOL_SOCKET,
//                     SO_BROADCAST,
//                     (void *) &(info->permission),
//                     sizeof(info->permission));
//     if (rc != 0) {

//         WARNING("setsockopt");
//         return ERROR;
//     }

//     [> ブロードキャストのアドレス構造体を作成する <]
//     info->addr.sin_family      = AF_INET;
//     info->addr.sin_addr.s_addr = inet_addr(info->ipaddr);
//     info->addr.sin_port        = htons(info->port);

//     return SUCCESS;
// }

/*!
 * @brief      ソケットの終期化
 * @param[in]  info   ブロードキャスト情報
 * @return     成功ならば0、失敗ならば-1を返す。
 */
// void socket_finalize(bc_info_t *info) {
//     [> ソケット破棄 <]
//     if (info->sd != 0) close(info->sd);
//     info->msg = NULL;
// }

/*!
 * @brief      ブロードキャストを送信する
 * @param[in]  info   ブロードキャスト情報
 * @param[out] errmsg エラーメッセージ格納先
 * @return     成功ならば0、失敗ならば-1を返す。
 */
// int broadcast_sendmsg(bc_info_t *info) {
//     long sendmsg_len = 0;

//     [> ブロードキャストを送信し続ける <]
//     sendmsg_len = sendto(info->sd,
//                          info->msg,
//                          info->msg_len,
//                          0,
//                          (struct sockaddr *) &(info->addr),
//                          sizeof(info->addr));
//     if (sendmsg_len != info->msg_len) {
//         WARNING("sendto");
//         return ERROR;
//     }

//     return SUCCESS;
// }

/*!
 * @brief      ブロードキャストを送信する
 * @param[in]  info   ブロードキャスト情報
 * @param[out] errmsg エラーメッセージ格納先
 * @return     成功ならば0、失敗ならば-1を返す。
 */
// int broadcast_sender(bc_info_t *info) {
//     int rc = 0;

//     [> ソケットの初期化 <]
//     rc = socket_initialize(info);
//     if (rc != 0) return ERROR;

//     [> ブロードキャストを送信する <]
//     // rc = broadcast_sendmsg(info);

//     [> ソケットの終期化 <]
//     // socket_finalize(info);

//     return SUCCESS;
// }

/*!
 * @brief      初期化処理。IPアドレスとポート番号を設定する。
 * @param[in]  argc   コマンドライン引数の数
 * @param[in]  argv   コマンドライン引数
 * @param[out] info   ブロードキャスト情報
 * @param[out] errmsg エラーメッセージ格納先
 * @return     成功ならば0、失敗ならば-1を返す。
 */
// int socket_init_state(char *ipaddr, unsigned short port, bc_info_t *info) {

//     memset(info, 0, sizeof(bc_info_t));
//     info->ipaddr     = ipaddr;
//     info->port       = port;
//     info->msg        = NULL;
//     info->msg_len    = 0;
//     info->sd         = 0;
//     info->permission = 1;

//     [> ソケットの初期化 <]
//     if (socket_initialize(info) == ERROR) {
//         return ERROR;
//     }

//     return SUCCESS;
// }

/*!
 * @brief   main routine
 * @return  成功ならば0、失敗ならば-1を返す。
 */
// // int main(int argc, char *argv[]) {
// //     int rc         = 0;
// //     bc_info_t info = {0};
// //     char errmsg[BUFSIZ];

// //     rc = initialize(argc, argv, &info, errmsg);
// //     if (rc != 0) {
// //         fprintf(stderr, "Error: %s\n", errmsg);
// //         return (-1);
// //     }

// //     rc = broadcast_sender(&info, errmsg);
// //     if (rc != 0) {
// //         fprintf(stderr, "Error: %s\n", errmsg);
// //         return (-1);
// //     }

// //     return (0);
// // }
