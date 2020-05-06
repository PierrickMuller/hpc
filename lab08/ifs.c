/* 06.05.2020 - Sydney Hauke - HPC - REDS - HEIG-VD */

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <time.h>

#include "image.h"
#include "ifs.h"
#include "random.h"

#define TRANSFORM_SIZE      6

/* Sierpinski triangle */
#define NB_ST_TRANSFORMS    3
float ST_TRANSFORMS[NB_ST_TRANSFORMS][TRANSFORM_SIZE] = {
{
    0.5f,  0.0f,   0.0f,
    0.0f,  0.5f,   0.0f,
},
{
    0.5f,  0.0f,   0.5f,
    0.0f,  0.5f,   0.0f,
},
{
    0.5f,  0.0f,   0.25f,
    0.0f,  0.5f,   0.5f,
}};

/* Barnsley's fern */
#define NB_BF_TRANSFORMS    4
float BF_TRANSFORMS[NB_BF_TRANSFORMS][TRANSFORM_SIZE] = {
{
    0.0f,  0.0f,   0.0f,
    0.0f,  0.16f,   0.0f,
},
{
    0.2f,  -0.26f,   0.0f,
    0.23f,  0.22f,   1.6f,
},
{
    -0.15f,  0.28f,   0.0f,
    0.26f,  0.24f,   0.44f,
},
{
    0.85f,  0.04f,   0.0f,
    -0.04f,  0.85f,   1.6f,
}};

struct ifs_t {
    float width;
    float height;

    float center_x;
    float center_y;

    size_t nb_transforms;
    float *transforms;
};

/* Sierpinski triangle IFS */
const struct ifs_t st_ifs = {
    .width  = 1.0f,
    .height = 1.0f,

    .center_x = 0.5f,
    .center_y = 0.5f,

    .nb_transforms = NB_ST_TRANSFORMS,
    .transforms = (float*)ST_TRANSFORMS,
};

/* Barnley's fern IFS */
const struct ifs_t bf_ifs = {
    .width  = 6.0f,
    .height = 10.0f,

    .center_x = 0.0f,
    .center_y = 4.5f,

    .nb_transforms = NB_BF_TRANSFORMS,
    .transforms = (float*)BF_TRANSFORMS, 
};

static void affine_transform(float *x, float *y, const float transform[6]);
static size_t __ifs(struct img_t *img, const struct ifs_t *ifs, size_t passes, size_t width, size_t height);

void ifs(char *pathname, size_t passes, size_t min_width)
{
    const struct ifs_t *i = &bf_ifs;
    struct img_t *fractal_img;
    size_t width, height;
    float aspect_ratio;

    /* Fractals have a specific aspect ratio. The resulting image
     * must have the same aspect ratio as well */
    aspect_ratio = i->height / i->width;
    width = min_width;
    height = width * aspect_ratio;

    fractal_img = allocate_image(width, height, COMPONENT_GRAYSCALE);

    /* Generate fractal image */
    size_t points_drawn = __ifs(fractal_img, i, passes, width, height);
    printf("Number of points drawn : %lu\n", points_drawn);

    save_image(pathname, fractal_img);
    printf("Fractal saved as %s (%lu, %lu)\n", pathname, width, height);

    free_image(fractal_img);
}

static size_t __ifs(struct img_t *img, const struct ifs_t *ifs, size_t passes, size_t width, size_t height)
{
    /* TODO : do multiple instances of this algorithm so that the number of points generated 
     * per second increases. Use SIMD instructions. */

    /* This is the real number of iterations we do to draw the fractal */
    size_t count_points = width*height*passes;

    /* We start from the origin point */
    float p_x = 0.0f;
    float p_y = 0.0f;
    struct xorshift32_state rand_state; // random number generator

    srand(time(NULL));

    /* Choose a random starting state for the random number generator */
    rand_state.a = rand();

    for (size_t iterations = count_points; iterations != 0; iterations--) {
        /* Randomly choose an affine transform */
        size_t rand_idx = xorshift32(&rand_state) % ifs->nb_transforms;

        /* Apply choosen affine transform */
        affine_transform(&p_x, &p_y, &ifs->transforms[rand_idx*TRANSFORM_SIZE]);

        /* Check if point lies inside the boundaries of our image */
        if (p_x < ifs->center_x+ifs->width/2 && p_x > ifs->center_x-ifs->width/2 && 
            p_y < ifs->center_y+ifs->height/2 && p_y > ifs->center_y-ifs->height/2) 
        {
            float left = ifs->center_x-ifs->width/2;
            float bottom = ifs->center_y-ifs->height/2;

            /* If point lies in the image, save and colour it in white */
            img->data[(height-1-(int32_t)((p_y-bottom)/ifs->height*height))*width + 
            (int32_t)((p_x-left)/ifs->width*width)] = UINT8_MAX;
        }
    }

    return count_points;
}

static void affine_transform(float *x, float *y, const float transform[TRANSFORM_SIZE])
{
    *x = transform[0] * *x + transform[1] * *y + transform[2];
    *y = transform[3] * *x + transform[4] * *y + transform[5];
}