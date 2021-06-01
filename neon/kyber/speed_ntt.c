#include <stdio.h>
#include <arm_neon.h>
#include <sys/random.h>
#include <string.h>
#include <assert.h>
#include "polyvec.h"
#include "params.h"
#include "neon_ntt.h"
#include "reduce.h"
#include "print.h"
#include <time.h>

// clang ntt.c reduce.c neon_ntt.c speed_ntt.c -o neon_ntt -O3 -g3 -Wall -Werror -Wextra -Wpedantic -lpapi
// gcc   ntt.c reduce.c neon_ntt.c speed_ntt.c -o neon_ntt -O3 -g3 -Wall -Werror -Wextra -Wpedantic -lpapi

const int16_t zetas[128] = {
    -1044, -758, -359, -1517, 1493, 1422, 287, 202,
    -171, 622, 1577, 182, 962, -1202, -1474, 1468,
    573, -1325, 264, 383, -829, 1458, -1602, -130,
    -681, 1017, 732, 608, -1542, 411, -205, -1571,
    1223, 652, -552, 1015, -1293, 1491, -282, -1544,
    516, -8, -320, -666, -1618, -1162, 126, 1469,
    -853, -90, -271, 830, 107, -1421, -247, -951,
    -398, 961, -1508, -725, 448, -1065, 677, -1275,
    -1103, 430, 555, 843, -1251, 871, 1550, 105,
    422, 587, 177, -235, -291, -460, 1574, 1653,
    -246, 778, 1159, -147, -777, 1483, -602, 1119,
    -1590, 644, -872, 349, 418, 329, -156, -75,
    817, 1097, 603, 610, 1322, -1285, -1465, 384,
    -1215, -136, 1218, -1335, -874, 220, -1187, -1659,
    -1185, -1530, -1278, 794, -1510, -854, -870, 478,
    -108, -308, 996, 991, 958, -1460, 1522, 1628};

/*************************************************
* Name:        fqmul
*
* Description: Multiplication followed by Montgomery reduction
*
* Arguments:   - int16_t a: first factor
*              - int16_t b: second factor
*
* Returns 16-bit integer congruent to a*b*R^{-1} mod q
**************************************************/
static int16_t fqmul(int16_t a, int16_t b) {
  return montgomery_reduce((int32_t)a*b);
}

/*************************************************
* Name:        ntt
*
* Description: Inplace number-theoretic transform (NTT) in Rq.
*              input is in standard order, output is in bitreversed order
*
* Arguments:   - int16_t r[256]: pointer to input/output vector of elements of Zq
**************************************************/
static
void ntt(int16_t r[256]) {
  unsigned int len, start, j, k;
  int16_t t, zeta;

  k = 1;
  for(len = 128; len >= 2; len >>= 1) {
    for(start = 0; start < 256; start = j + len) {
      zeta = zetas[k++];
      for(j = start; j < start + len; j++) {
        t = fqmul(zeta, r[j + len]);
        r[j + len] = r[j] - t;
        r[j] = r[j] + t;
      }
    }
  }
}

/*************************************************
* Name:        invntt_tomont
*
* Description: Inplace inverse number-theoretic transform in Rq and
*              multiplication by Montgomery factor 2^16.
*              Input is in bitreversed order, output is in standard order
*
* Arguments:   - int16_t r[256]: pointer to input/output vector of elements of Zq
**************************************************/
static
void invntt(int16_t r[256]) {
  unsigned int start, len, j, k;
  int16_t t, zeta;
  const int16_t f = 1441; // mont^2/128
  // int32_t overflow;

  k = 127;
  for(len = 2; len <= 128; len <<= 1) {
    for(start = 0; start < 256; start = j + len) {
      zeta = zetas[k--];
      for(j = start; j < start + len; j++) {
        t = r[j];
        /* overflow = (int32_t) t + r[j + len];
        if (overflow > INT16_MAX) 
        {
          printf("%d: [%d] overflow: %d\n", len, j ,overflow);
        }
        else if (overflow < INT16_MIN)
        {
          printf("%d: [%d] overflow: %d\n", len, j, overflow);
        }
        
        if (len > 4) {
        }
        else{
          r[j] = (t + r[j + len]);
        } */
        r[j] = barrett_reduce(t + r[j + len]);
        r[j + len] = r[j + len] - t;
        r[j + len] = fqmul(zeta, r[j + len]);
      }
    }
  }

  for(j = 0; j < 256; j++)
    r[j] = fqmul(r[j], f);
}


/* static 
int compare_strict(int16_t *a, int16_t *b, int length, const char *string)
{
  int i, j, count = 0;
  int16_t aa, bb;
  for (i = 0; i < length; i += 8)
  {
    for (j = i; j < i + 8; j++)
    {
      aa = a[j];
      bb = b[j];
      if (aa != bb)
      {
        printf("%d: %d != %d\n", j, aa, bb);
        count += 1;
      }

      if (count > 8)
      {
        printf("%s Incorrect!!\n", string);
        return 1;
      }
    }
  }
  if (count)
  {
    printf("%s Incorrect!!\n", string);
    return 1;
  }
  printf("OK: %s\n", string);
  return 0;
} */

static 
int compare(int16_t *a, int16_t *b, int length, const char *string)
{
  int i, j, count = 0;
  int16_t aa, bb;
  for (i = 0; i < length; i += 8)
  {
    for (j = i; j < i + 8; j++)
    {
      aa = a[j]; //% KYBER_Q;
      bb = b[j]; //% KYBER_Q;
      if (aa != bb)
      {
        if ((aa + KYBER_Q == bb) || (aa - KYBER_Q == bb))
        {
          printf("%d: %d != %d: %d != %d ", j, a[j], b[j], aa, bb);
          printf(": OK\n");
        }
        else
        {
          printf("%d: %d != %d: %d != %d: Error\n", j, a[j], b[j], aa, bb);
          count++;
        }
      }
      if (count > 16)
      {
        printf("%s Incorrect!!\n", string);
        return 1;
      }
    }
  }
  if (count)
  {
    printf("%s Incorrect!!\n", string);
    return 1;
  }

  return 0;
}

static 
void poly_reduce(poly *r)
{
  unsigned int i;
  for (i = 0; i < KYBER_N; i++)
    r->coeffs[i] = barrett_reduce(r->coeffs[i]);
}

static 
void poly_ntt(poly *r)
{
  ntt(r->coeffs);
  poly_reduce(r);
}

static 
void poly_neon_ntt(poly *r)
{
  neon_ntt(r->coeffs);
  poly_reduce(r);
}

static
void print_array(const int16_t *buf, int buflen, const char *string)
{
    printf("%s: [", string);
    for (int i = 0; i < buflen; i++)
    {
        printf("%d, ", buf[i]);
    }
    printf("]\n");
}

static
void reduce(int16_t r[256])
{
  for (int i = 0; i < 256; i++)
  {
    r[i] = barrett_reduce(r[i]);
  }
}

#define TEST1 1000000
#define TEST2 0

int main(void)
{
  int16_t r_gold[256], r1[256], r2[256];
  int retval;
  int comp = 0;
  clock_t start, end; 

  getrandom(r_gold, sizeof(r_gold), 0);
  for (int i = 0; i < 256; i++)
  {
    r_gold[i] = barrett_reduce(r_gold[i]);
    assert(r_gold[i] < INT16_MAX);
    assert(INT16_MIN < r_gold[i]);
  }
  memcpy(r1, r_gold, sizeof(r_gold));
  memcpy(r2, r_gold, sizeof(r_gold));

  // Test NTT
  // retval = PAPI_hl_region_begin("c_ntt");
  start = clock();
  for (int j = 0; j < TEST1; j++)
  {
    ntt(r_gold);
  }
  end =  clock() - start;
  print("ntt:", ((double) end)/CLOCKS_PER_SEC);
  // retval = PAPI_hl_region_end("c_ntt");

  // retval = PAPI_hl_region_begin("neon_ntt");
  start = clock();
  for (int j = 0; j < TEST1; j++)
  {
    neon_ntt(r1);
  }
  end =  clock() - start;
  print("neon_ntt:", ((double) end)/CLOCKS_PER_SEC);
  // retval = PAPI_hl_region_end("neon_ntt");

  comp = compare(r_gold, r1, 256, "c_ntt vs neon_ntt");
  if (comp)
    return 1;

  reduce(r_gold);
  reduce(r1);

  // Test INTT
  // retval = PAPI_hl_region_begin("c_invntt");
  start = clock();
  for (int j = 0; j < TEST1; j++)
  {
    invntt(r_gold);
  }
  end =  clock() - start;
  print("invntt:", ((double) end)/CLOCKS_PER_SEC);
  // retval = PAPI_hl_region_end("c_invntt");

  // retval = PAPI_hl_region_begin("neon_invntt");
  start = clock();
  for (int j = 0; j < TEST1; j++)
  {
    neon_invntt(r1);
  }
  end =  clock() - start;
  print("neon_invntt:", ((double) end)/CLOCKS_PER_SEC);
  // retval = PAPI_hl_region_end("neon_invntt");

  comp = compare(r_gold, r1, 256, "c_invntt vs neon_invntt");
  if (comp)
    return 1;

  if (retval)
    return 1;

  polyvec rvec_gold;
  polyvec rvec_test;

  for (int k = 0; k < TEST2; k++)
  {
    for (int j = 0; j < KYBER_K; j++)
    {
      getrandom(rvec_gold.vec[j].coeffs, 256, 0);
      for (int i = 0; i < 256; i++)
      {
        rvec_gold.vec[j].coeffs[i] %= KYBER_Q;
      }
      memcpy(rvec_test.vec[j].coeffs, rvec_gold.vec[j].coeffs, sizeof(r_gold));

      poly_ntt(&rvec_gold.vec[j]);
      poly_neon_ntt(&rvec_test.vec[j]);

      comp |= compare(rvec_gold.vec[j].coeffs, rvec_test.vec[j].coeffs, 256, "rvec_gold vs rvec_test");

      if (comp)
      {
        printf("ERROR poly_ntt\n");
        return 1;
      }
    }
  }

  return 0;
}
