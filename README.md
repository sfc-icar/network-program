## ネットワークプログラミング
# 完全に理解した

----

## 目標

ネットワーク通信（L4）を行う
コードを眺めて雰囲気を知る

----

<img src="https://i.imgur.com/vJOy0Hg.png" height=600px />

----

HackMD

[![hackmd-github-sync-badge](https://hackmd.io/6HTr3vzZSoaV4oA1xqBO2g/badge)](https://hackmd.io/6HTr3vzZSoaV4oA1xqBO2g)

スライド

[https://hackmd.io/@akakou/rkzn05KdP#/](https://hackmd.io/@akakou/rkzn05KdP#/)

Github

[https://github.com/sfc-icar/network-program](https://github.com/sfc-icar/network-program)

----

## アジェンダ

- 環境構築
- TCP通信
- TCP通信（バインドなし）
- UDP通信
- getaddrinfo
- まとめ


----

## 環境構築

----

## Mac

※未確認

```shell=
# homebrewをインストール
# https://qiita.com/zaburo/items/29fe23c1ceb6056109fd
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install.sh)"

# dockerのインストール
# https://qiita.com/nemui_/items/ed753f6b2eb9960845f7
brew install docker
brew cask install docker
open /Applications/Docker.app

# サンプルプログラムのインストール
git clone https://github.com/sfc-icar/network-program
cd network-program
```

----

## Windows

1. WSLを設定
  a. ここの[リンク](https://qiita.com/yoshige/items/dbc85b048fba51e597ee)を参考に
2. サンプルプログラムのダウンロード

※未確認

```shell=
# WSLのターミナルで以下を実行
git clone https://github.com/sfc-icar/network-program
cd network-program
sh install.sh
```

---

## TCP

----

<img src="https://raw.githubusercontent.com/sfc-icar/network-program/main/img/1.jpg" height=600px />

----

#### クライアントサンプル

1. 11111番ポートに対してTCP接続を確立した後にサーバーからのデータを待つ
2. データを受け取ると、標準出力へと受信データを表示して終了

----

tcp/client.c

```c=
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


int
main(void)
{
    struct sockaddr_in server;
    int sock;
    char buf[32];
    int n;

    /* ソケットの作成 */
    sock = socket(AF_INET, SOCK_STREAM, 0);

    /* 接続先指定用構造体の準備 */
    server.sin_family = AF_INET;
    server.sin_port = htons(11111);
    // server.sin_len = sizeof(server); /* Linuxでは不要 */

    /* 127.0.0.1はlocalhost */
    inet_pton(AF_INET, "127.0.0.1", &server.sin_addr);

    /* sockaddr_in 構造体の長さを指定(Linuxでは不要) */
    // server.size_len = sizeof(server);

    /* サーバに接続 */
    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) != 0){
    	perror("connect");
	return 1;
    }

    /* サーバからデータを受信 */
    memset(buf, 0, sizeof(buf));
    n = read(sock, buf, sizeof(buf));

    printf("%d, %s\n", n, buf);
    
    /* socketの終了 */
    close(sock);


    return 0;
}
```


<!-- ----

#### 技術的な話

- socketシステムコールを使用してソケットを作成
    - TCPによる通信プログラムを書く場合、
        - 第1引数はAF_INET
        - 第2引数はSOCK_STREAM
- connectシステムコールでTCP接続を確立
    - 第2引数に接続相手情報を指定
 -->

----


#### サーバサンプル

1. サーバー側はポート11111番で2本のTCP接続を受け付ける
2. 1本目のTCP接続に対してはHELLOを書き込み、2本目にはHOGEと書き込む

----

tcp/server.c

```c=
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>


int
main(void)
{
    int sock0;
    struct sockaddr_in addr;
    struct sockaddr_in client;
    socklen_t len;
    int sock1, sock2;

    /* ソケットの作成 */
    sock0 = socket(AF_INET, SOCK_STREAM, 0);


    /*** ソケットの設定 ***/

    /* IPv4を利用することを指定 */
    addr.sin_family = AF_INET;

    /* ポート番号は11111 */
    addr.sin_port = htons(11111);

    /* 任意のインターフェースを指定 */
    addr.sin_addr.s_addr = INADDR_ANY;

    /* sockaddr_in構造体のサイズを設定(Linuxでは不要) */
    // addr.sin_len = sizeof(addr);

    /* ソケットをインターフェースとポートに関連付け */
    bind(sock0, (struct sockaddr *)&addr, sizeof(addr));

    /* TCPクライアントからの接続要求を持てる状態にする */
    listen(sock0, 5);

    /* TCPクライアントからの接続要求を受け付ける(1回目) */
    len = sizeof(client);
    sock1 = accept(sock0, (struct sockaddr *)&client, &len);

    /* 6文字送信('H', 'E', 'L', 'L', 'O', '\0') */
    write(sock1, "HELLO", 6);

    /* TCPセッションの終了 */
    close(sock1);


    /* TCPクライアントからの接続要求を受け付ける(2回目) */
    sock2 = accept(sock0, (struct sockaddr *)&client, &len);

    /* 5文字送信 */
    write(sock2, "HOGE", 5);

    /* TCPセッション2の終了 */
    close(sock2);

    
    /* listenするsocketの終了 */
    close(sock0);

    return 0;
}

```

<!-- ----

#### 技術的な話

- bindシステムコールを使用して待ち受け用のIPアドレスとTCPポート番号を設定
    - listenシステムコールによってTCP SYNを受け付けるようする
- acceptシステムコールがTCP接続が確立し終わったTCP接続から新たなソケットを生成する -->


---

## TCP 2

----

- BINDという操作は、IP + ポートとソケットを紐付ける。
- プログラマがBINDしなければどうなるか
    - カーネルが自動で適当なポートを決める。

----

#### クライアントサンプル

1. ユーザがサーバのポートを入力
2. サーバの入力したポートに接続
3. サーバから送られてくるデータを表示

----

tcp2/client.c

```c=
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


int
main(void)
{
    struct sockaddr_in server;
    int sock;
    char buf[32];
    int n;
    int port; /* 本に記載なし */


    /* ポート番号の入力(本に記載なし) */
    puts("[client] Enter server port");
    scanf("%d", &port);

    /* ソケットの作成 */
    sock = socket(AF_INET, SOCK_STREAM, 0);

    /* 接続先指定用構造体の準備 */
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    // server.sin_len = sizeof(server); /* Linuxでは不要 */

    /* 127.0.0.1はlocalhost */
    inet_pton(AF_INET, "127.0.0.1", &server.sin_addr);

    /* sockaddr_in 構造体の長さを指定(Linuxでは不要) */
    // server.size_len = sizeof(server);

    /* サーバに接続 */
    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) != 0){
    	perror("connect");
	return 1;
    }

    /* サーバからデータを受信 */
    memset(buf, 0, sizeof(buf));
    n = read(sock, buf, sizeof(buf));

    printf("[client] %d, %s\n", n, buf);
    
    /* socketの終了 */
    close(sock);


    return 0;
}

```


----

#### サーバサンプル

1. バインドせずにconnectする
2. ポートを取得して表示
3. サーバの接続を待ち、接続されたら `HOGE` と送る 

----

tcp2/server.c

```c=
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
```

---

## UDP


----

<img src="https://raw.githubusercontent.com/sfc-icar/network-program/main/img/2.jpg" height=600px />

----

#### receiveサンプル

1. UDP用のソケットを作成
3. ポート番号11111番

----

udp/receive.c

```c=
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
    bindaddr.sin_port = htons(11111); /* ポート番号は11111 */
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
```

----

#### sendサンプル
1. IPアドレスはlocalhost(127.0.0.1)、ポート番号は11111番
2. 送信しているデータ「'T', 'E', 'S', 'T', '\0'」

----

udp/send.c

```c=
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
```

---

## getaddrinfo

----

- アプリケーションが名前解決を行なっていることを忘れがち
- 名前解決は重要

----

- 名前解決とは
    - ```www.example.com```と通信を行うときにそのアドレスのIPアドレスを得ること
- ソケットを利用したプログラムではgetaddrinfoを利用する


---

## IPv6とIPv4の
## どちらを使うべきか（おまけ）

----

- 名前解決によって得られるIPアドレスは1つだけとは限らない
    - 名前解決の結果、IPv4とIPv6の両方得られた場合どちらのアドレスファミリを利用するかはプログラマに任される

----

- Appleがやったこと
    - IPv4とIPv6の両方に対する名前解決を行いつつ、最初に届いた結果IPv4であれば25ミリ秒待ってからTCP接続を試みる
    - IPv６を優先的に使うように工夫

----

##### RFC 6555 - Happy Eyeballs

1. getaddrinfo()を利用して名前解決を行う
2. getaddrinfo()から受け取ったIPアドレスリストの順番に接続を試みる
3. 接続の試みが短時間で確立しない場合、異なるアドレスファミリの最初のIPアドレスリストでの接続を試みる。この場合、直前に失敗したのがIPv6であればIPv4を試す
4. 最初に確立した接続を利用する

----

- 複数のTCP接続を並行して確立しようとしつつ、最初に成功したTCP接続を利用
    - IPv6とIPv4のどちらを利用するのがいいのか自動的に判断する試み

---

## まとめ
---
### あー、そーゆーことね
### 完全に理解した