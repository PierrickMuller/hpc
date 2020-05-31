#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <xmmintrin.h>

struct xorshift32_state {
    __m128i a;
};

void xorshift32(float * rand,struct xorshift32_state *state,size_t nb_transforms);
