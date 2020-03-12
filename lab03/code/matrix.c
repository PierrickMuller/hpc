#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "bench.h"

#define DATA_FILE "/dev/urandom"

struct matrix {
    size_t row;
    size_t col;

    double **data;
};

/* initializes matrix with random values in range [0 .. RANDOM_DOUBLE_MAX] */
static void matrix_data_init(struct matrix *m)
{
    FILE *fdata;

    fdata = fopen(DATA_FILE, "rb");
    if (!fdata) {
        fprintf(stderr, "[%s] fopen error\n", __func__);
        perror(__func__);
        exit(EXIT_FAILURE);
    }

    /* matrix data init */
    for (size_t i = 0; i < m->row; i++) {
        for (size_t j = 0; j < m->col; j++) {
            /* fill m->data with random values */
            uint64_t rand;

            if (fread(&rand , sizeof (uint64_t), 1, fdata) != 1) {
                fprintf(stderr, "[%s] fread error\n", __func__);
                exit(EXIT_FAILURE);
            }

            /* restrict them to range [0 .. RANDOM_DOUBLE_MAX] */
            m->data[i][j] = (double)rand / (double)UINTMAX_MAX * RANDOM_DOUBLE_MAX; 
        }
    }
}

/* allocates matrix memory and initialize it with random values */
struct matrix *matrix_init(const size_t row, const size_t col)
{
    struct matrix *m;
    FILE *fdata;

    fdata = fopen(DATA_FILE, "rb");
    if (!fdata) {
        fprintf(stderr, "[%s] fopen error\n", __func__);
        perror(__func__);
        exit(EXIT_FAILURE);
    }

    /* matrix struct allocation */
    m = malloc(sizeof(struct matrix));
    if (!m) {
        fprintf(stderr, "[%s] malloc error\n", __func__);
        perror(__func__);
        exit(EXIT_FAILURE);
    }

    m->row = row;
    m->col = col;

    /* matrix rows allocation */
    m->data = (double **)malloc(sizeof(double *) * row);
    if (!m->data) {
        fprintf(stderr, "[%s] malloc error\n", __func__);
        perror(__func__);
        exit(EXIT_FAILURE);
    }

    /* matrix columns allocation */
    for (size_t i = 0; i < m->row; i++) {
        m->data[i] = (double *)malloc(sizeof(double) * col);

        if (!m->data[i]) {
            fprintf(stderr, "[%s] malloc error\n", __func__);
            perror(__func__);
            exit(EXIT_FAILURE);
        }
    }

    matrix_data_init(m);

    return m;
}

void matrix_clear(struct matrix *m)
{
    for (size_t i = 0; i < m->row; i++)
        free(m->data[i]);

    free(m->data);
    free(m);
}

void matrix_zero(struct matrix *m)
{
    for (size_t i = 0; i < m->row; i++)
        for (size_t j = 0; j < m->col; j++)
            m->data[i][j] = 0.0;
}

void matrix_print(const struct matrix *m, const char *name)
{
    for (size_t i = 0; i < m->row; i++) {
        fprintf(stderr, "\n[%s - %s] ", __func__, name);

        for (size_t j = 0; j < m->col; j++)
            fprintf(stderr, "%f ", m->data[i][j]);
    }

    fprintf(stderr, "\n");
}

/* returns true if matrices dimensions correct, false otherwise */
static bool matrix_check(const struct matrix *m1, const struct matrix *m2, const struct matrix *m3)
{
    /* checking m1 & m2 dimensions */
    if (m1->col != m2->row)
        return false;
    /* checking m3 dimensions */
    if (m3->row != m1->row)
        return false;
    if (m3->col != m2->col)
        return false;

    return true;
}

/* performs matrix multiplication.
 * result stored in m3 (must be previously allocated)
 * returns false if matrices don't have correct dimensions
 * less efficient version */
bool matrix_mult(const struct matrix *m1, const struct matrix *m2, struct matrix *m3)
{
    if (!matrix_check(m1, m2, m3))
        return false;

    /* actual product */
    for (size_t i = 0; i < m1->row; i++)
        for (size_t j = 0; j < m2->col; j++)
            for (size_t k = 0; k < m1->col; k++)
                m3->data[i][j] += m1->data[i][k] * m2->data[k][j];

    return true;
}

/* performs matrix multiplication.
 * result stored in m3 (must be previously allocated)
 * returns false if matrices don't have correct dimensions
 * more efficient version */
bool matrix_mult_xchg(const struct matrix *m1, const struct matrix *m2, struct matrix *m3)
{
    if (!matrix_check(m1, m2, m3))
        return false;

    for (size_t i = 0; i < m1->row; i++)
        for (size_t k = 0; k < m1->col; k++)
            for (size_t j = 0; j < m2->col; j++)
                m3->data[i][j] += m1->data[i][k] * m2->data[k][j];

    return true;
}
