#include "common.h"

#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

void usage(int argc, char **argv) {
    printf("usage: %s <server IP> <server port>\n", argv[0]);
    printf("example: %s 127.0.0.1 51511\n", argv[0]);
    exit(EXIT_FAILURE);
}

void treatMsg(char *msg) {
    int tamanho = strlen(msg);
    for (int i = 0; i < tamanho; i++) {
        if (msg[i] == '\\' && msg[i + 1] == 'n') {
            msg[i] = '\n';
            for (int a = i + 1; a < tamanho; a++) {
                msg[a] = msg[a + 1];
            }
            tamanho--;
        }
    }
}
void showServerResponse(char *b,int *kill) {
    printf("< ");
    for (int i = 0; i < strlen(b); i++) {
        if(b[i]=='K'){
            *kill=1;
            break;
        }
        printf("%c", b[i]);
        if (b[i] == '\n' && i != strlen(b) - 1) {
            printf("< ");
        }
    }
}
#define BUFSZ 500

int main(int argc, char **argv) {
    if (argc < 3) {
        usage(argc, argv);
    }

    struct sockaddr_storage storage;
    if (0 != addrparse(argv[1], argv[2], &storage)) {
        usage(argc, argv);
    }

    int s;
    s = socket(storage.ss_family, SOCK_STREAM, 0);
    if (s == -1) {
        logexit("socket");
    }
    struct sockaddr *addr = (struct sockaddr *)(&storage);
    if (0 != connect(s, addr, sizeof(storage))) {
        logexit("connect");
    }

    char addrstr[BUFSZ];
    addrtostr(addr, addrstr, BUFSZ);

    printf("connected to %s\n", addrstr);

    char buf[BUFSZ];
    int killProgram=0;
    while (!killProgram) {
        memset(buf, 0, BUFSZ);
        printf("> ");
        fgets(buf, BUFSZ - 1, stdin);
        treatMsg(buf);
        size_t count = send(s, buf, strlen(buf), 0);
        if (count <= 1) {
            close(s);
            break;
        }
        if (count != strlen(buf)) {
            logexit("send");
        }

        memset(buf, 0, BUFSZ);

        recv(s, buf, BUFSZ, 0);

        showServerResponse(buf,&killProgram);
    }
    close(s);

    exit(EXIT_SUCCESS);
}