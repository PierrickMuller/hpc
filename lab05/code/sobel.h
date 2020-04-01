/* 09.03.2020 - Sydney Hauke - HPC - REDS - HEIG-VD */

#pragma once

struct img_t *edge_detection(const struct img_t *input_img);

void rgb_to_grayscale(const struct img_t *img, struct img_t *result);

void gaussian_filter(const struct img_t *img, struct img_t *res_img, const uint16_t *kernel);

/* H+V gradients and thresholding */
void sobel_filter(const struct img_t *img, struct img_t *res_img, 
                  const int16_t *v_kernel, const int16_t *h_kernel);