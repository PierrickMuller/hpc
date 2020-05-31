/* 06.05.2020 - Sydney Hauke - HPC - REDS - HEIG-VD */

#pragma once

#include <stddef.h>
#include <xmmintrin.h>
#include <smmintrin.h>

void ifs(char *pathname, size_t passes, size_t min_width);
/*
void print128_num(__m128 var);
void print128_numi(__m128i var);
*/
