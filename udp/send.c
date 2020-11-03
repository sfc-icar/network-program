#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(void) {
    int sock;
    struct sockaddr_in s;
    int n;

    /* ソケットを生成 */
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    s.sin_family = AF_INET; /* IPv4 */
    s.sin_port = htons(11111); /* ポート番号は11111 */

    /* 宛先IPアドレスとして 127.0.0.1 を設定 */
    n = inet_pton(AF_INET, "127.0.0.1", &s.sin_addr);
    if (n < 1) {
        perror("inet_pton");
        return 1;
    }

    /* UDPパケットを送信 */
    n = sendto(sock, "TEST", 5, 0, (struct sockaddr *)&s, sizeof(s));
    if (n < 1) {
        perror("sendto");
        return 1;
    }

    /* ソケットをclose */
    close(sock);

    return 0;
}