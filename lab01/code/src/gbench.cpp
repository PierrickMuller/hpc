/* 26.01.16 - Xavier Ruppen - HPC - REDS - HEIG-VD */

#include <stdio.h>
#include <stdlib.h>
#include "benchmark/benchmark.h"
#include "bench.h"

extern "C" {
#include "matrix.h"
}

class MatrixFixture : public benchmark::Fixture {
public :
    void SetUp(const ::benchmark::State&)
    {
        m1 = matrix_init(MATRIX1_ROW, MATRIX1_COL);
        m2 = matrix_init(MATRIX2_ROW, MATRIX2_COL);
        m3 = matrix_init(MATRIX1_ROW, MATRIX2_COL);
    }

    void TearDown(const ::benchmark::State&)
    {
        matrix_clear(m1);
        matrix_clear(m2);
        matrix_clear(m3);
    }

protected :
    struct matrix *m1, *m2, *m3;
};


BENCHMARK_F(MatrixFixture, matrix_mult)(benchmark::State& state) {
    while (state.KeepRunning()) {
        matrix_zero(m3);

        if (!matrix_mult(m1, m2, m3)) {
            fprintf(stderr, "[%s] wrong matrix sizes\n", __func__);
            exit(EXIT_FAILURE);
        }
    }
}

BENCHMARK_F(MatrixFixture, matrix_mult_xchg)(benchmark::State& state) {
    while (state.KeepRunning()) {
        matrix_zero(m3);

        if (!matrix_mult_xchg(m1, m2, m3)) {
            fprintf(stderr, "[%s] wrong matrix sizes\n", __func__);
            exit(EXIT_FAILURE);
        }
    }
}

BENCHMARK_MAIN();
