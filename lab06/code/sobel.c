#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "image.h"
#include "sobel.h"

#define GAUSSIAN_KERNEL_SIZE    3
#define SOBEL_KERNEL_SIZE       3

#define SOBEL_BINARY_THRESHOLD  150  // in the range 0 to uint8_max (255)

const int16_t sobel_v_kernel[SOBEL_KERNEL_SIZE*SOBEL_KERNEL_SIZE] = {
    -1, -2, -1,
     0,  0,  0,
     1,  2,  1,
};

const int16_t sobel_h_kernel[SOBEL_KERNEL_SIZE*SOBEL_KERNEL_SIZE] = {
    -1,  0,  1,
    -2,  0,  2,
    -1,  0,  1,
};

const uint16_t gauss_kernel[GAUSSIAN_KERNEL_SIZE*GAUSSIAN_KERNEL_SIZE] = {
    1, 2, 1,
    2, 4, 2,
    1, 2, 1,
};

void rgb_to_grayscale(const struct img_t *img, struct img_t *result)
{
    size_t index = 0;

    for (size_t y = 0; y < result->height; y++) {
        for (size_t x = 0; x < result->width; x++) {
            result->data[y * result->width + x] =
                FACTOR_R * img->data[index + R_OFFSET] +
                FACTOR_G * img->data[index + G_OFFSET] +
                FACTOR_B * img->data[index + B_OFFSET];

            index += img->components;
        }
    }
}

void gaussian_filter(const struct img_t *img, struct img_t *res_img, const uint16_t *kernel)
{

    const uint16_t gauss_ponderation = 16;

    for (size_t x = 0; x < img->width; x++) {
        for (size_t y = 0; y < img->height; y++) {
            uint16_t pix_acc = 0;

            if (x < GAUSSIAN_KERNEL_SIZE/2 ||
                x >= img->width - GAUSSIAN_KERNEL_SIZE/2 ||
                y < GAUSSIAN_KERNEL_SIZE/2 ||
                y >= img->height - GAUSSIAN_KERNEL_SIZE/2) {
                    res_img->data[y * res_img->width + x] = img->data[y * img->width + x];
                    continue;
            }

            for (size_t ky = 0; ky < GAUSSIAN_KERNEL_SIZE; ky++) {
                for (size_t kx = 0; kx < GAUSSIAN_KERNEL_SIZE; kx++) {
                    pix_acc += kernel[ky * GAUSSIAN_KERNEL_SIZE + kx] *
                               img->data[(y - 1 + ky) * img->width + (x - 1 + kx)];
                }
            }

            res_img->data[y * res_img->width + x] = pix_acc / gauss_ponderation;
        }
    }
}

void sobel_filter(const struct img_t *img, struct img_t *res_img, 
                  const int16_t *v_kernel, const int16_t *h_kernel)
{
    for (size_t x = 0; x < img->width; x++) {
        for (size_t y = 0; y < img->height; y++) {
            int16_t Gx = 0;
            int16_t Gy = 0;
            
            if (x < SOBEL_KERNEL_SIZE/2 ||
                x >= img->width - SOBEL_KERNEL_SIZE/2 ||
                y < SOBEL_KERNEL_SIZE/2 ||
                y >= img->height - SOBEL_KERNEL_SIZE/2) {
                    res_img->data[y * res_img->width + x] = img->data[y * img->width + x];
                    continue;
            }

            for (size_t ky = 0; ky < SOBEL_KERNEL_SIZE; ky++) {
                for (size_t kx = 0; kx < SOBEL_KERNEL_SIZE; kx++) {
                    Gx += h_kernel[ky * SOBEL_KERNEL_SIZE + kx] *
                          img->data[(y - 1 + ky) * img->width + (x - 1 + kx)];

                    Gy += v_kernel[ky * SOBEL_KERNEL_SIZE + kx] *
                          img->data[(y - 1 + ky) * img->width + (x - 1 + kx)];
                }
            }

            res_img->data[y * res_img->width + x] = 
                sqrt(Gx*Gx + Gy*Gy) > SOBEL_BINARY_THRESHOLD ? UINT8_MAX : 0;
        }
    }
}

struct img_t *edge_detection(const struct img_t *input_img)
{
    struct img_t *res_img;
    struct img_t *gauss_img;
    struct img_t *gs_img;

    if (input_img->components < COMPONENT_RGB) {
        fprintf(stderr, "[%s] only accepts images with RGB(A) components", __func__);
        return NULL;
    }

    gs_img = allocate_image(input_img->width, input_img->height, COMPONENT_GRAYSCALE);
    gauss_img = allocate_image(gs_img->width, gs_img->height, gs_img->components);
    res_img = allocate_image(gs_img->width, gs_img->height, gs_img->components);

    rgb_to_grayscale(input_img, gs_img);
    gaussian_filter(gs_img, gauss_img, gauss_kernel);
    sobel_filter(gauss_img, res_img, sobel_v_kernel, sobel_h_kernel);

    free_image(gauss_img);
    free_image(gs_img);

    return res_img;
}