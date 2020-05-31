/* 06.05.2020 - Sydney Hauke - HPC - REDS - HEIG-VD */

#include "random.h"
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>

uint32_t xorshift32(struct xorshift32_state *state)
{
    uint32_t x = state->a;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    return state->a = x;
}
