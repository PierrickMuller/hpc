#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <limits.h>

#ifndef CLIENT_FILE
#define CLIENT_FILE "client_data.bin"
#endif

#ifndef FILE_SIZE
#define FILE_SIZE (1 << 30) /* 1 GB */
#endif

static void client_recv(const int sock, const int file)
{
    int ret;
    size_t buf_size = 8192;
    if(FILE_SIZE < buf_size)
      buf_size = FILE_SIZE;

    uint8_t buf[buf_size];

    for (size_t i = 0; i < FILE_SIZE/buf_size; i++) {

        if ((ret = recv(sock, &buf, buf_size, 0)) < 0) {
            perror("recv() error");
            exit(EXIT_FAILURE);
        }

        if (ret < 0) {
            perror("listen() error");
            exit(EXIT_FAILURE);
        }

        if (ret > buf_size) {
            fprintf(stderr, "[%s] Error: sent %d bytes (expected 1)\n", __func__, ret);
            exit(EXIT_FAILURE);
        }


        ret = write(file, &buf, ret);

        if (ret < 0) {
            perror("write() error");
            exit(EXIT_FAILURE);
        }

        if (ret >buf_size) {
            fprintf(stderr, "[%s] Error: %d bytes written (expected 1)", __func__, ret);
            exit(EXIT_FAILURE);
        }
    }
    printf("[%s] written %ld bytes\n", __func__, (long)FILE_SIZE);


}

void client_start(const char *ipv4_srv, unsigned short port_srv)
{
    int file;
    int sock;
    struct sockaddr_in addr_srv;

    /* file init */
    file = open(CLIENT_FILE, O_RDWR|O_CREAT|O_DSYNC, 0666);
    if (file < 0) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    /* socket init */
    if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        perror("Error creating client socket");
        exit(EXIT_FAILURE);
    }

    memset(&addr_srv, 0, sizeof(addr_srv));
    addr_srv.sin_family = AF_INET;
    addr_srv.sin_addr.s_addr = inet_addr(ipv4_srv);
    addr_srv.sin_port = htons(port_srv);

    if (connect(sock, (struct sockaddr *) &addr_srv, sizeof(addr_srv)) < 0) {
        perror("connect() error");
        exit(EXIT_FAILURE);
    }

    /* connected */
    client_recv(sock, file);

    close(sock);
    close(file);
}
