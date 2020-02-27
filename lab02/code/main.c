#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/wait.h>

#include "server.h"
#include "client.h"

#define IPV4_SRV "127.0.0.1"
#define PORT 12345

#ifndef FILE_SIZE 
#define FILE_SIZE (1 << 30) /* 1 GB */
#endif

static void time_diff(const struct timespec *start, const struct timespec *end, struct timespec *diff)
{
    if ((end->tv_nsec - start->tv_nsec) < 0) {
        diff->tv_sec = end->tv_sec - start->tv_sec - 1;
        diff->tv_nsec = 1E9 + end->tv_nsec - start->tv_nsec;
    }

    else {
        diff->tv_sec = end->tv_sec - start->tv_sec;
        diff->tv_nsec = end->tv_nsec - start->tv_nsec;
    }
}

static void time_report(const struct timespec *time)
{
    long nsec_total;
    double rate;

    nsec_total = time->tv_sec * 1E9 + time->tv_nsec;
    rate = (double)FILE_SIZE / (double)nsec_total * (double)1E9;

    printf("[%s] Report:\n", __func__);
    printf("[%s] -------\n", __func__);
    printf("[%s] size: %d bytes\n", __func__, FILE_SIZE);
    printf("[%s] time: %ld sec and %ld ns\n", __func__, time->tv_sec, time->tv_nsec);
    printf("[%s] rate: %f bytes/sec\n", __func__, rate);
}

int main(int argc, char *argv[])
{
    pid_t pid;
    int status_client;
    struct timespec start, end, diff;

    /* arg parsing */
    if (argc != 1) {
        fprintf(stderr, "Usage:  %s\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    clock_gettime(CLOCK_MONOTONIC_RAW, &start);

    /* create server and client */
    pid = fork();

    if (pid) {
        srv_start(PORT);

        /* wait for client to complete its task */
        if (waitpid(pid, &status_client, 0) < 0) {
            perror("waitpid() error");
            exit(EXIT_FAILURE);
        }

        if (status_client != EXIT_SUCCESS) {
            fprintf(stderr, "[%s] Client exited with status %d\n", __func__, status_client);
            exit(EXIT_FAILURE);
        }

        /* benchmark results */
        clock_gettime(CLOCK_MONOTONIC_RAW, &end);
        time_diff(&start, &end, &diff);
        time_report(&diff);
    }
    else
        client_start(IPV4_SRV, PORT);

    return EXIT_SUCCESS;
}
