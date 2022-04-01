#include "common.h"
#include "location.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/types.h>

#define BUFSZ 500
#define SERVER_ID 65535

void usage(int argc, char **argv) {
    printf("usage: %s <v4|v6> <server port>\n", argv[0]);
    printf("example: %s v4 51511\n", argv[0]);
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv) {
    if (argc < 3) {
        usage(argc, argv);
    }

    struct sockaddr_storage storage;
    if (0 != server_sockaddr_init(argv[1], argv[2], &storage)) {
        usage(argc, argv);
    }

    int s;
    s = socket(storage.ss_family, SOCK_STREAM, 0);
    if (s == -1) {
        logexit("socket");
    }

    int enable = 1;
    if (0 != setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int))) {
        logexit("setsockopt");
    }

    struct sockaddr *addr = (struct sockaddr *)(&storage);
    if (0 != bind(s, addr, sizeof(storage))) {
        logexit("bind");
    }

    if (0 != listen(s, 10)) {
        logexit("listen");
    }

    char addrstr[BUFSZ];
    addrtostr(addr, addrstr, BUFSZ);
    printf("bound to %s, waiting connections\n", addrstr);

    while (1) {
        struct sockaddr_storage cstorage;
        struct sockaddr *caddr = (struct sockaddr *)(&cstorage);
        socklen_t caddrlen = sizeof(cstorage);

        int csock = accept(s, caddr, &caddrlen);
        if (csock == -1) {
            logexit("accept");
        }

        char caddrstr[BUFSZ];
        addrtostr(caddr, caddrstr, BUFSZ);
        printf("[log] connection from %s\n", caddrstr);

        while (1) {
            char buf[BUFSZ];
            memset(buf, 0, BUFSZ);
            size_t count = recv(csock, buf, BUFSZ - 1, 0);
            printf("[msg] %s, %d bytes: %s", caddrstr, (int)count, buf);

            while (buf[strlen(buf) - 1] != '\n') {
                char buf2[BUFSZ];
                memset(buf2, 0, BUFSZ);
                count += recv(csock, buf2, BUFSZ - 1, 0);
                strcat(buf, buf2);
            }
            if (strlen(buf)<= 1) {
                close(csock);
                break;
            } else {
                // inicio

                char *msg;
                msg = strtok(buf, " \n");
                while (msg != NULL) {
                    if (0 == strcmp(msg, "add")) {
                        msg = strtok(NULL, " \n");
                        int x = atoi(msg);
                        msg = strtok(NULL, " \n");
                        int y = atoi(msg);
                        addLocation(x, y);
                    } else if (0 == strcmp(msg, "rm")) {
                        msg = strtok(NULL, " \n");
                        int x = atoi(msg);
                        msg = strtok(NULL, " \n");
                        int y = atoi(msg);
                        removeLocation(x, y);
                    } else if (0 == strcmp(msg, "list")) {
                        listLocations();
                    } else if (0 == strcmp(msg, "query")) {
                        msg = strtok(NULL, " \n");
                        int x = atoi(msg);
                        msg = strtok(NULL, " \n");
                        int y = atoi(msg);
                        query(x, y);
                    } else if (0 == strcmp(msg, "kill")) {
                        sprintf(client_log.buffer,"K\n");
                        addLogMsg(client_log.buffer);
                        send(csock, getLogMsg(), strlen(client_log.msg), 0);
                        close(csock);
                        logexit("kill");
                    }
                    msg = strtok(NULL, " \n");
                }

                // sprintf(client_log.msg, "remote endpoint: %.100s\n",
                // caddrstr);

                // fim
                count = send(csock, getLogMsg(), strlen(client_log.msg), 0);
                if (count != strlen(getLogMsg())) {
                    logexit("send");
                    close(csock);
                }
                clearClientLog();
            }
        }
    }

    exit(EXIT_SUCCESS);
}