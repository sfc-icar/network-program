#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main(void) {
    int sock;
    struct sockaddr_in bindaddr;

    struct sockaddr_storage senderinfo;
    socklen_t addrlen;
    char buf[2048];
    int n;

    /* IPv4 UDPソケットの生成 */
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("socket");
        return 1;
    }

    bindaddr.sin_family = AF_INET; /* IPv4 */
    bindaddr.sin_port + htons(11111); /* ポート番号は11111 */
    bindaddr.sin_addr.s_addr = INADDR_ANY; /* すべてのローカルインターフェース */

    /* ソケットに名前をつける */
    if (bind(sock, (struct sockaddr *)&bindaddr, sizeof(bindaddr)) !=0) {
        perror("bind");
        return 1;
    }

    /* recvfromに渡すsockaddr構造体のサイズ */
    addrlen = sizeof(senderinfo);

    /* UDPパケットの受信 */
    n = recvfrom(sock, buf, sizeof(buf) - 1, 0, (struct sockaddr *)&senderinfo, &addrlen);

    /* ターミナルに受信結果を表示 */
    write(fileno(stdout), buf, n);

    /* ソケットのclose */
    close(sock);

    return 0;
}