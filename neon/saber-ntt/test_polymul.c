#include "params.h"
#include "consts256.h"
#include "neon_ntt.h"
#include "poly.h"
#include "polyvec.h"
#include "util.h"

#include <stdlib.h>
#include <stdio.h>

static void schoolbook(poly *r, poly *a, poly *b)
{
  int16_t t[2 * KEM_N] = {0};
  for (int i = 0; i < KEM_N; i++)
  {
    for (int j = 0; j < KEM_N; j++)
    {
      t[i + j] = (t[i + j] + a->coeffs[i] * b->coeffs[j]) % KEM_Q;
    }
  }

  for (int i = 0; i < KEM_N; i++)
  {
    r->coeffs[i] = (t[i] - t[i + KEM_N]) % KEM_Q;
  }
}

int main(void)
{
  int i, j, k;
  int error = 0;
  polyvec a[KEM_K], b, r, r_gold;
  poly t;

  // Nothing secret, just dumb random number
  int16_t aa = 4712;
  int16_t bb = 4914;
  for (j = 0; j < KEM_K; j++)
  {
    for (k = 0; k < KEM_K; k++)
    {
      for (i = 0; i < POLY_N; i++)
      {
        a[j].vec[k].coeffs[i] = aa % KEM_Q;
        aa = (aa * 0x1337) + i;
      }
    }
    for (i = 0; i < POLY_N; i++)
    {
      b.vec[j].coeffs[i] = bb % 5;
      bb = (bb * 0x1415) + i;
    }
  }

  // Test Matrix Vector Mul  and NTT Matrix Vector Mul
  for (i = 0; i < KEM_K; i++)
  {
    schoolbook(&r_gold.vec[i], &a[i].vec[0], &b.vec[0]);
    for (j = 1; j < KEM_K; j++)
    {
      schoolbook(&t, &a[i].vec[j], &b.vec[j]);
      for (k = 0; k < KEM_N; k++)
      {
        r_gold.vec[i].coeffs[k] += t.coeffs[k];
      }
    }
  }

  polyvec_matrix_vector_mul(&r, a, &b, 0);

  for (i = 0; i < KEM_K; i++)
  {
    error |= compare_array(r_gold.vec[i].coeffs, r.vec[i].coeffs);
  }

  if (error)
  {
    printf("polyvec_matrix_vector_mul error\n");
    return 1;
  }
  printf("polyvec_matrix_vector_mul OK\n");

  // Test InnerProd and NTT InnerProd
  schoolbook(&r_gold.vec[0], &a[0].vec[0], &b.vec[0]);
  for (i = 1; i < KEM_K; i++)
  {
    schoolbook(&t, &a[0].vec[i], &b.vec[i]);
    for (k = 0; k < KEM_N; k++)
    {
      r_gold.vec[0].coeffs[k] += t.coeffs[k];
    }
  }

  polyvec_iprod(&r.vec[0], &a[0], &b);

  error |= compare_array(r_gold.vec[0].coeffs, r.vec[0].coeffs);

  if (error)
  {
    printf("polyvec_iprod error\n");
    return 1;
  }
  printf("polyvec_iprod OK\n");

  return 0;
}
