#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

int main(void) {
    char *hostname = "www.example.com";
    char *service = "http";

    struct addrinfo hints, *res0, *res;
    int err;
    int sock;
    int n;

    memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_family = PF_UNSPEC; /* IPv4とIPv6両方を取得 */
    if ((err = getaddrinfo(hostname, service, &hints, &res0)) != 0) {
        printf("error %d\n", err);
        return 1;
    }

    for (res=res0; res!=NULL; res=res->ai_next) {
        sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        if (sock < 0) {
            continue;
        }

        if (connect(sock, res->ai_addr, res->ai_addrlen) != 0) {
            close(sock);
            continue;
        }

        break;
    }

    if (res == NULL) {
        /* 有効な接続ができなかった */
        printf("failed\n");
        return 1;
    }

    freeaddrinfo(res0);

    /* サーバからデータを受信 */
    char req[] = "GET / HTTP/1.1\r\nHost: www.example.com\r\n\r\n";
    write(sock, req, sizeof(req));

    n = 1;
    while(n != 0) {
        char resp[64];
        memset(resp, 0, sizeof(resp));
        n = read(sock, resp, sizeof(resp));
        printf("%s", resp);
    }
    
    /* socketの終了 */
    close(sock);

    return 0;
}
