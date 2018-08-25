#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>

#include <global.h>
#include <message.h>
#include <socket.h>
/*!
 * @brief      ソケットの初期化
 * @param[in]  info   ブロードキャスト情報
 * @param[out] errmsg エラーメッセージ格納先
 * @return     成功ならば0、失敗ならば-1を返す。
 */
static int socket_initialize(bc_info_t *info) {
    int rc = 0;

    /* ソケットの生成 : UDPを指定する */
    info->sd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (info->sd < 0) {
        WARNING("socket");
        return ERROR;
    }

    /* ブロードキャスト機能を有効にする */
    rc = setsockopt(info->sd,
                    SOL_SOCKET,
                    SO_BROADCAST,
                    (void *) &(info->permission),
                    sizeof(info->permission));
    if (rc != 0) {

        WARNING("setsockopt");
        return ERROR;
    }

    /* ブロードキャストのアドレス構造体を作成する */
    info->addr.sin_family      = AF_INET;
    info->addr.sin_addr.s_addr = inet_addr(info->ipaddr);
    info->addr.sin_port        = htons(info->port);

    return SUCCESS;
}

/*!
 * @brief      ソケットの終期化
 * @param[in]  info   ブロードキャスト情報
 * @return     成功ならば0、失敗ならば-1を返す。
 */
void socket_finalize(bc_info_t *info) {
    /* ソケット破棄 */
    if (info->sd != 0) close(info->sd);
    info->msg = NULL;

    return;
}

/*!
 * @brief      ブロードキャストを送信する
 * @param[in]  info   ブロードキャスト情報
 * @param[out] errmsg エラーメッセージ格納先
 * @return     成功ならば0、失敗ならば-1を返す。
 */
int broadcast_sendmsg(bc_info_t *info) {
    long sendmsg_len = 0;

    /* ブロードキャストを送信し続ける */
    sendmsg_len = sendto(info->sd,
                         info->msg,
                         info->msg_len,
                         0,
                         (struct sockaddr *) &(info->addr),
                         sizeof(info->addr));
    if (sendmsg_len != info->msg_len) {
        WARNING("sendto");
        return ERROR;
    }

    return SUCCESS;
}

/*!
 * @brief      ブロードキャストを送信する
 * @param[in]  info   ブロードキャスト情報
 * @param[out] errmsg エラーメッセージ格納先
 * @return     成功ならば0、失敗ならば-1を返す。
 */
int broadcast_sender(bc_info_t *info) {
    int rc = 0;

    /* ソケットの初期化 */
    rc = socket_initialize(info);
    if (rc != 0) return ERROR;

    /* ブロードキャストを送信する */
    // rc = broadcast_sendmsg(info);

    /* ソケットの終期化 */
    // socket_finalize(info);

    return SUCCESS;
}

/*!
 * @brief      初期化処理。IPアドレスとポート番号を設定する。
 * @param[in]  argc   コマンドライン引数の数
 * @param[in]  argv   コマンドライン引数
 * @param[out] info   ブロードキャスト情報
 * @param[out] errmsg エラーメッセージ格納先
 * @return     成功ならば0、失敗ならば-1を返す。
 */
int socket_init_state(char *ipaddr, unsigned short port, bc_info_t *info) {

    memset(info, 0, sizeof(bc_info_t));
    info->ipaddr     = ipaddr;
    info->port       = port;
    info->msg        = NULL;
    info->msg_len    = 0;
    info->sd         = 0;
    info->permission = 1;

    /* ソケットの初期化 */
    if (socket_initialize(info) == ERROR) {
        return ERROR;
    }

    return SUCCESS;
}

/*!
 * @brief   main routine
 * @return  成功ならば0、失敗ならば-1を返す。
 */
// int main(int argc, char *argv[]) {
//     int rc         = 0;
//     bc_info_t info = {0};
//     char errmsg[BUFSIZ];

//     rc = initialize(argc, argv, &info, errmsg);
//     if (rc != 0) {
//         fprintf(stderr, "Error: %s\n", errmsg);
//         return (-1);
//     }

//     rc = broadcast_sender(&info, errmsg);
//     if (rc != 0) {
//         fprintf(stderr, "Error: %s\n", errmsg);
//         return (-1);
//     }

//     return (0);
// }
