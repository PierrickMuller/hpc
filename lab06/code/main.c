#include <stdlib.h>
#include <stdio.h>

#include "image.h"
#include "sobel.h"

#define EXPECTED_NB_ARGS 3

void print_usage()
{
    printf("Please provide an input image and an output image\n");
}

int main(int argc, char *argv[])
{
    struct img_t *input_img;
    struct img_t *result_img;

    if (argc != EXPECTED_NB_ARGS) {
        fprintf(stderr, "Invalid number of arguments\n");
        print_usage();
        return EXIT_FAILURE;
    }

    input_img = load_image(argv[1]);
    result_img = edge_detection(input_img);
    save_image(argv[2], result_img);

    free_image(input_img);
    free_image(result_img);

    return EXIT_SUCCESS;
}