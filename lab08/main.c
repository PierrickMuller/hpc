/* 06.05.2020 - Sydney Hauke - HPC - REDS - HEIG-VD */

#include <stdlib.h>
#include <stdio.h>

#include "ifs.h"
#define EXPECTED_NB_ARGS 2

void print_usage()
{
    printf("Please provide an output image pathname\n");
}

int main(int argc, char *argv[])
{
    if (argc != EXPECTED_NB_ARGS) {
        fprintf(stderr, "Invalid number of arguments\n");
        print_usage();
        return EXIT_FAILURE;
    }

    ifs(argv[1], 150, 600);

    return EXIT_SUCCESS;
}