/* 06.05.2020 - Sydney Hauke - HPC - REDS - HEIG-VD */

#include "random.h"
#include "ifs.h"

void xorshift32(float * rand,struct xorshift32_state *state,size_t nb_transforms)
{

  __m128 x;
  __m128i tempx2,tempx3;

  unsigned int d = nb_transforms;

  tempx2 = state->a;

// On effectue les transformations nécéssaires
  tempx3 = tempx2;
  tempx2 = _mm_slli_epi32(tempx2,13);
  tempx3 = _mm_xor_si128(tempx3,tempx2);

  tempx2 = tempx3;
  tempx2 = _mm_srli_epi32(tempx2,17);
  tempx3 = _mm_xor_si128(tempx3,tempx2);

  tempx2 = tempx3;
  tempx2 = _mm_slli_epi32(tempx2,5);
  tempx3 = _mm_xor_si128(tempx3,tempx2);

  state->a = tempx3;

// Si on peut effectuer un modulo facilement, donc si le nombre de transformation est une puissance de deux, on le fait, et sinon on utilise une boucle pour le réaliser.
  if(nb_transforms & 0x1)
  {
    x = _mm_cvtepi32_ps(tempx3);
    _mm_store_ps(rand,x);
    for(int i= 0; i < 4; i++)
    {
      rand[i] = ((unsigned int)rand[i] % nb_transforms);
    }

  }
  else
  {
    tempx2 = _mm_set1_epi32((d-1));
    tempx3 = _mm_and_si128(tempx3,tempx2);
    x = _mm_cvtepi32_ps(tempx3);
    _mm_store_ps(rand,x);
  }

}
