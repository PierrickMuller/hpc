#pragma once

struct matrix;

struct matrix *matrix_init(const size_t row, const size_t col);
void matrix_clear(struct matrix *m);
void matrix_zero(struct matrix *m);
void matrix_print(const struct matrix *m, const char *name);
bool matrix_mult(const struct matrix *m1, const struct matrix *m2, struct matrix *m3);
bool matrix_mult_xchg(const struct matrix *m1, const struct matrix *m2, struct matrix *m3);
