/* 06.05.2020 - Sydney Hauke - HPC - REDS - HEIG-VD */
/* Maximum 4 transformations , partir du principe qu'il y a 4*/


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
    float* transforms;
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

static void affine_transform(__m128 *x, __m128 *y, float* transform, float* rand_idx, __m128 * baseArray);
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

    /* This is the real number of iterations we do to draw the fractal */
    size_t count_points = width*height*passes;

    /* We start from the origin point */
    __m128 p_x;
    __m128 p_y;
    float rand_idx[4];// = {0,0,0,0};
    p_x = _mm_set_ps1(0.0f);
    p_y = _mm_set_ps1(0.0f);
    struct xorshift32_state rand_state; // random number generator

    srand(time(NULL));

    /* Choose a random starting state for the random number generator */
    rand_state.a = _mm_set_epi32(abs(rand()),abs(rand()),abs(rand()),abs(rand()));


    __m128 left = _mm_set_ps1(ifs->center_x-ifs->width/2);
    __m128 bottom = _mm_set_ps1(ifs->center_y-ifs->height/2);
    __m128 heighXheight = _mm_set_ps1(ifs->height/height);
    __m128 widthXwhidth = _mm_set_ps1(ifs->width/width);
    /*
    __m128 imagecontenant = _mm_set_ps1(UINT8_MAX);
    __m128 val1 = _mm_set_ps1(ifs->center_x+ifs->width/2);
    __m128 val2 = _mm_set_ps1(ifs->center_x-ifs->width/2);
    __m128 val3 = _mm_set_ps1(ifs->center_y+ifs->height/2);
    __m128 val4 = _mm_set_ps1(ifs->center_y-ifs->height/2);
    */
    float y[4];
    float x[4];
    /*
    float result[4];
    __m128 cmp1;
    __m128 cmp2;
    __m128 cmp3;
    __m128 cmp4;
    __m128 cmpFinal;
    */
    __m128 calcX;
    __m128 calcY;

    __m128 test[8];
    // On remplit une fois le tableau contenant toutes les transformations
    for(size_t i = 0; i < 4; i++)
    {
      test[i] = _mm_set_ps(*(&ifs->transforms[0] + i*TRANSFORM_SIZE + 3),*(&ifs->transforms[0] + i*TRANSFORM_SIZE + 2),*(&ifs->transforms[0] + i*TRANSFORM_SIZE + 1),*(&ifs->transforms[0] + i*TRANSFORM_SIZE));
      test[i+4] = _mm_set_ps(0.0f,0.0f,*(&ifs->transforms[0] + i*TRANSFORM_SIZE + 5 ),*(&ifs->transforms[0] + i*TRANSFORM_SIZE + 4));
    }
    for (size_t iterations = count_points; iterations != 0; iterations--) {

      // Génération des valeurs aléatoires
        xorshift32(rand_idx,&rand_state, ifs->nb_transforms);

        /* Apply choosen affine transform */
        affine_transform(&p_x, &p_y, &ifs->transforms[0], rand_idx, test);

        /* Check if point lies inside the boundaries of our image*/
/*
        cmp1 = _mm_cmplt_ps(p_x,val1);
        cmp2 = _mm_cmpgt_ps(p_x,val2);
        cmp3 = _mm_cmplt_ps(p_y,val3);
        cmp4 = _mm_cmpgt_ps(p_y,val4);
        cmpFinal = _mm_and_ps(cmp1,cmp2);
        cmpFinal = _mm_and_ps(cmp3,cmpFinal);
        cmpFinal = _mm_and_ps(cmp4,cmpFinal);
        cmpFinal = _mm_and_ps(imagecontenant,cmpFinal);
        _mm_store_ps(result,cmpFinal);*/

        // On précalcule les points
        calcX = _mm_sub_ps(p_x,left);
        calcY = _mm_sub_ps(p_y,bottom);
        calcX = _mm_div_ps(calcX,widthXwhidth);
        calcY = _mm_div_ps(calcY,heighXheight);


        _mm_store_ps(y,calcY);
        _mm_store_ps(x,calcX);

        // On écrit l'image
        for(int i = 0; i < 4;i++)
        {
            img->data[(height-1-(int32_t)(y[i]))*width +
            (int32_t)((x[i]))] = UINT8_MAX;
        }
    }

    return count_points*4;
}

static void affine_transform(__m128 *x, __m128 *y,  float* transform, float* rand_idx, __m128 * baseArray)
{

    __m128 temp;
    __m128 xtemp;
    __m128 ytemp;
    __m128 row03[8];

    // En fonction des nombres aléatoires reçus, on remplit les row correspondantes
    for(int i = 0; i < 4; i++)
    {
      row03[i] = baseArray[(int)rand_idx[i]];
      row03[i+4] = baseArray[(int)(4 + rand_idx[i])];
    }

    // On utilise cet macro deux fois afin de transformer nos rows en colonnes utilisable pour la suite
    _MM_TRANSPOSE4_PS (row03[0], row03[1], row03[2], row03[3]);
    _MM_TRANSPOSE4_PS (row03[4], row03[5], row03[6], row03[7]);


    // On calcule les valeurs des points.
    xtemp = *x;
    ytemp = *y;
    *x = _mm_mul_ps(row03[0],xtemp);
    temp = _mm_mul_ps(row03[1],ytemp);
    *x = _mm_add_ps(*x,temp);
    *x = _mm_add_ps(*x,row03[2]);

    *y = _mm_mul_ps(row03[3],xtemp);
    temp = _mm_mul_ps(row03[4],ytemp);
    *y = _mm_add_ps(*y,temp);
    *y = _mm_add_ps(*y,row03[5]);

}


//Fonctions de debug
/*void print128_num(__m128 var)
{
    float *val = (float*) &var;//can also use uint32_t instead of 16_t
    printf("Numerical: %f %f %f %f  \n",
           val[0], val[1], val[2], val[3]);
}

void print128_numi(__m128i var)
{
    int *val = (int*) &var;//can also use uint32_t instead of 16_t
    printf("Numerical: %d %d %d %d  \n",
           val[0], val[1], val[2], val[3]);
}*/
