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
        size_t height = result->height;
        size_t width = result->width;
        int nb_components = img->components;
        for (size_t y = 0; y < height; y++) {
                for (size_t x = 0; x < width; x++) {
                        result->data[y * width + x] =
                                FACTOR_R * img->data[index + R_OFFSET] +
                                FACTOR_G * img->data[index + G_OFFSET] +
                                FACTOR_B * img->data[index + B_OFFSET];


                        index += nb_components;//img->components;
                }
        }
}
void gaussian_filter(const struct img_t *img, struct img_t *res_img, const uint16_t *kernel)
{

        const uint16_t gauss_ponderation = 16;
        int img_width = img->width;
        int res_img_width = res_img->width;
        int height = img->height;

        for (size_t y = 0; y < height; y++) {
                uint16_t pix_acc_fut = 0;
                uint16_t pix_acc_fut_2 = 0;
                for (size_t x = 0; x < img_width; x++) {
                        uint16_t pix_acc = 0;

                        uint8_t temp = 0;


                        if (x < GAUSSIAN_KERNEL_SIZE/2 ||
                            x >= img_width - GAUSSIAN_KERNEL_SIZE/2 ||
                            y < GAUSSIAN_KERNEL_SIZE/2 ||
                            y >= height - GAUSSIAN_KERNEL_SIZE/2) {

                                res_img->data[y * res_img_width + x] = img->data[y*res_img_width + x];
                                continue;
                        }

                        //SI second pixel d'une ligne
                        if(x < GAUSSIAN_KERNEL_SIZE/2 + 1)
                        {
                                //si second pixel de seconde ligne
                                for (size_t ky = 0; ky < GAUSSIAN_KERNEL_SIZE; ky++)
                                {
                                        for (size_t kx = 0; kx < GAUSSIAN_KERNEL_SIZE; kx++)
                                        {

                                                temp = img->data[(y - 1 + ky) * img_width + (x - 1 + kx)];
                                                pix_acc += kernel[ky * GAUSSIAN_KERNEL_SIZE + kx] *
                                                           temp;
                                                if(kx != 0)
                                                {
                                                        pix_acc_fut += kernel[ky * GAUSSIAN_KERNEL_SIZE + (kx-1)]*
                                                                       temp;
                                                        if(kx == 2)
                                                        {
                                                                pix_acc_fut_2 += kernel[ky * GAUSSIAN_KERNEL_SIZE + (kx-2)]*
                                                                                 temp;
                                                        }
                                                }
                                        }
                                }
                        }
                        //Si avant-dernier pixel d'une ligne
                        else if (x >= img_width - GAUSSIAN_KERNEL_SIZE/2 - 1 )
                        {
                                pix_acc += pix_acc_fut;
                                pix_acc_fut = 0;
                                for (size_t ky = 0; ky < GAUSSIAN_KERNEL_SIZE; ky++)
                                {
                                        pix_acc += kernel[ky * GAUSSIAN_KERNEL_SIZE + 2] *
                                                   img->data[(y - 1 + ky) * img_width + (x + 1)];
                                }
                        }
                        else
                        {
                                pix_acc += pix_acc_fut;
                                pix_acc_fut = pix_acc_fut_2;
                                pix_acc_fut_2 = 0;
                                for (size_t ky = 0; ky < GAUSSIAN_KERNEL_SIZE; ky++)
                                {

                                        temp = img->data[(y - 1 + ky) * img_width + (x + 1)];

                                        pix_acc += kernel[ky * GAUSSIAN_KERNEL_SIZE + 2] *
                                                   temp;

                                        pix_acc_fut += kernel[ky * GAUSSIAN_KERNEL_SIZE + (2-1)]*
                                                       temp;
                                        pix_acc_fut_2 += kernel[ky * GAUSSIAN_KERNEL_SIZE + (2-2)]*
                                                         temp;

                                }
                        }

                        res_img->data[y * res_img_width + x] = pix_acc / gauss_ponderation;
                }
        }

}

void sobel_filter(const struct img_t *img, struct img_t *res_img,
                  const int16_t *v_kernel, const int16_t *h_kernel)
{

        int img_width = img->width;
        int res_img_width = res_img->width;
        int height = img->height;
        for (size_t y = 0; y < height; y++) {
                int16_t Gx_fut = 0;
                int16_t Gy_fut = 0;
                int16_t Gx_fut_2 = 0;
                int16_t Gy_fut_2 = 0;
                for (size_t x = 0; x < img_width; x++) {
                        int16_t Gx = 0;
                        int16_t Gy = 0;
                        uint8_t temp = 0;

                        if (x < SOBEL_KERNEL_SIZE/2 ||
                            x >= img_width - SOBEL_KERNEL_SIZE/2 ||
                            y < SOBEL_KERNEL_SIZE/2 ||
                            y >= height - SOBEL_KERNEL_SIZE/2) {
                                res_img->data[y * res_img_width + x] = img->data[y * img_width + x];
                                continue;
                        }

                        /*for (size_t ky = 0; ky < SOBEL_KERNEL_SIZE; ky++) {
                                for (size_t kx = 0; kx < SOBEL_KERNEL_SIZE; kx++) {
                                        Gx += h_kernel[ky * SOBEL_KERNEL_SIZE + kx] *
                                              img->data[(y - 1 + ky) * img_width + (x - 1 + kx)];

                                        Gy += v_kernel[ky * SOBEL_KERNEL_SIZE + kx] *
                                              img->data[(y - 1 + ky) * img_width + (x - 1 + kx)];
                                }
                           }*/


                        if(x < SOBEL_KERNEL_SIZE/2 + 1)
                        {
                                //si second pixel de seconde ligne
                                for (size_t ky = 0; ky < SOBEL_KERNEL_SIZE; ky++)
                                {
                                        for (size_t kx = 0; kx < SOBEL_KERNEL_SIZE; kx++)
                                        {

                                                temp = img->data[(y - 1 + ky) * img_width + (x - 1 + kx)];
                                                Gx += h_kernel[ky * SOBEL_KERNEL_SIZE + kx] *
                                                      temp;
                                                Gy += v_kernel[ky * SOBEL_KERNEL_SIZE + kx] *
                                                      temp;

                                                if(kx != 0)
                                                {
                                                        Gx_fut += h_kernel[ky * SOBEL_KERNEL_SIZE + (kx-1)]*
                                                                  temp;
                                                        Gy_fut += v_kernel[ky * SOBEL_KERNEL_SIZE + (kx-1)]*
                                                                  temp;
                                                        if(kx == 2)
                                                        {

                                                                Gx_fut_2 += h_kernel[ky * SOBEL_KERNEL_SIZE + (kx-2)]*
                                                                            temp;
                                                                Gy_fut_2 += v_kernel[ky * SOBEL_KERNEL_SIZE + (kx-2)]*
                                                                            temp;
                                                        }
                                                }
                                        }
                                }
                        }
                        //Si avant-dernier pixel d'une ligne
                        else if (x >= img_width - SOBEL_KERNEL_SIZE/2 - 1 )
                        {
                                Gx += Gx_fut;
                                Gy += Gy_fut;
                                Gx_fut = 0;
                                Gy_fut = 0;
                                for (size_t ky = 0; ky < SOBEL_KERNEL_SIZE; ky++)
                                {
                                        temp = img->data[(y - 1 + ky) * img_width + (x + 1)];
                                        Gx += h_kernel[ky * SOBEL_KERNEL_SIZE + 2] *
                                              temp;
                                        Gy += v_kernel[ky * SOBEL_KERNEL_SIZE + 2] *
                                              temp;
                                }
                        }
                        else
                        {
                                Gx += Gx_fut;
                                Gy += Gy_fut;
                                Gx_fut = Gx_fut_2;
                                Gy_fut = Gy_fut_2;
                                Gx_fut_2 = 0;
                                Gy_fut_2 = 0;
                                for (size_t ky = 0; ky < SOBEL_KERNEL_SIZE; ky++)
                                {

                                        temp = img->data[(y - 1 + ky) * img_width + (x + 1)];

                                        /*pix_acc += kernel[ky * SOBEL_KERNEL_SIZE + 2] *
                                                   temp;

                                           pix_acc_fut += kernel[ky * SOBEL_KERNEL_SIZE + (2-1)]*
                                                       temp;
                                           pix_acc_fut_2 += kernel[ky * SOBEL_KERNEL_SIZE + (2-2)]*
                                                         temp;*/
                                        Gx += h_kernel[ky * SOBEL_KERNEL_SIZE + 2] *
                                              temp;

                                        Gx_fut += h_kernel[ky * SOBEL_KERNEL_SIZE + (2-1)]*
                                                  temp;
                                        Gx_fut_2 += h_kernel[ky * SOBEL_KERNEL_SIZE + (2-2)]*
                                                    temp;

                                        Gy += v_kernel[ky * SOBEL_KERNEL_SIZE + 2] *
                                              temp;

                                        Gy_fut += v_kernel[ky * SOBEL_KERNEL_SIZE + (2-1)]*
                                                  temp;
                                        Gy_fut_2 += v_kernel[ky * SOBEL_KERNEL_SIZE + (2-2)]*
                                                    temp;

                                }
                        }






                        res_img->data[y * res_img_width + x] =
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
