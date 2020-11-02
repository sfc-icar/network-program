#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h> /* for memset */
#include <arpa/inet.h> /* for inet_ntop */


int
main(void)
{
    int sock0;
    struct sockaddr_in serv;
    struct sockaddr_in client;
    socklen_t len;
    int sock1;
    char serv_name[16];
    int n;


    /* ソケットの作成 */
    sock0 = socket(AF_INET, SOCK_STREAM, 0);


    /* TCPクライアントからの接続要求を持てる状態にする */
    listen(sock0, 5);
    if (n != 0) {
        perror("[server] listen");
    }


    /* getsocknameを使うための準備 */
    memset(&serv, 0, sizeof(serv));
    len = sizeof(serv);


    /* getsocknameでソケットに付随する情報を取得 */
    n = getsockname(sock0, (struct sockaddr *)&serv, &len);
    if (n != 0) {
        perror("[server] listen");
    }


    /* ソケットにつけられた「名前」をコンソールに表示 */
    memset(serv_name, 0, sizeof(serv_name));
    inet_ntop(serv.sin_family, &serv.sin_addr, serv_name, sizeof(serv_name));
    printf("[server] ipaddr=%s, port=%d\n", serv_name, ntohs(serv.sin_port));


    /* TCPクライアントからの接続要求を受け付ける(1回目) */
    len = sizeof(client);
    sock1 = accept(sock0, (struct sockaddr *)&client, &len);

    /* 6文字送信('H', 'E', 'L', 'L', 'O', '\0') */
    write(sock1, "HELLO", 6);

    /* TCPセッションの終了 */
    close(sock1);


    /* listenするsocketの終了 */
    close(sock0);

    return 0;
}

