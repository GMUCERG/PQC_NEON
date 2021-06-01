#include <stdint.h>
#include <immintrin.h>
#include "../polyvec.h"

uint64_t mask_ar[4]={~(0UL)};
__m256i mask_load;
__m256i mask,inv3_avx,inv9_avx,inv15_avx,int45_avx,int30_avx,int0_avx;

#include "toom-cook_4way.c"

void load_values() {
	int64_t i;

	int64_t inv3=43691;
	int64_t inv9=36409;
	int64_t inv15=61167;

	int64_t int45=45;
	int64_t int30=30;
	int64_t int0=0;

	int16_t inv3_avx_load[16], inv9_avx_load[16], inv15_avx_load[16], int45_avx_load[16], int30_avx_load[16], int0_avx_load[16];

	for(i=0;i<16;i++){
		inv3_avx_load[i]=inv3;
		inv9_avx_load[i]=inv9;
		inv15_avx_load[i]=inv15;
		int45_avx_load[i]=int45;
		int30_avx_load[i]=int30;
		int0_avx_load[i]=int0;
	}

	inv3_avx = _mm256_loadu_si256 ((__m256i const *) (&inv3_avx_load));
	inv9_avx = _mm256_loadu_si256 ((__m256i const *) (&inv9_avx_load));
	inv15_avx = _mm256_loadu_si256 ((__m256i const *) (&inv15_avx_load));
	int45_avx = _mm256_loadu_si256 ((__m256i const *) (&int45_avx_load));
	int30_avx = _mm256_loadu_si256 ((__m256i const *) (&int30_avx_load));
	int0_avx = _mm256_loadu_si256 ((__m256i const *) (&int0_avx_load));
	mask = _mm256_loadu_si256 ((__m256i const *)mask_ar);
}

void saber_matrix_vector_mul(polyvec *r, const polyvec a[KEM_K], const polyvec *v) {
  int64_t i,j,k;
  __m256i sk_avx[KEM_K][KEM_N/16];
  __m256i a1_avx_combined[NUM_POLY][NUM_POLY][AVX_N1];
  __m256i res_avx[NUM_POLY][AVX_N1];
  __m256i b_bucket[NUM_POLY][SCHB_N*4];
  __m256i c_bucket[2*SCM_SIZE*4];

  mask_ar[0]=~(0UL);mask_ar[1]=~(0UL);mask_ar[2]=~(0UL);mask_ar[3]=~(0UL);
  mask_load = _mm256_loadu_si256((__m256i const *)mask_ar);

  load_values();

  for(i=0;i<KEM_K;i++)
    for(j=0;j<KEM_N/16;j++)
      sk_avx[i][j] = _mm256_loadu_si256((__m256i const *)&v->vec[i].coeffs[16*j]);

  for(i=0;i<KEM_K;i++)
    for(j=0;j<KEM_K;j++)
      for(k=0;k<KEM_N/16;k++)
        a1_avx_combined[i][j][k] = _mm256_loadu_si256((__m256i const *)&a[i].vec[j].coeffs[k*16]);

  for(j=0;j<NUM_POLY;j++)
    TC_eval(sk_avx[j], b_bucket[j]);

  for(i=0;i<NUM_POLY;i++) {
    for(j=0;j<NUM_POLY;j++) {
      toom_cook_4way_avx_n1(a1_avx_combined[i][j], b_bucket[j], c_bucket, j);
    }
    TC_interpol(c_bucket, res_avx[i]);
  }

  for(i=0;i<KEM_K;i++)
    for(j=0;j<KEM_N/16;j++)
      _mm256_maskstore_epi32((int *)&r->vec[i].coeffs[16*j], mask_load, res_avx[i][j]);
}

void saber_iprod(poly *r, const polyvec *a, const polyvec *v) {
  int64_t i,j,k;
  __m256i sk_avx[KEM_K][KEM_N/16];
  __m256i a1_avx_combined[NUM_POLY][NUM_POLY][AVX_N1];
  __m256i res_avx[NUM_POLY][AVX_N1];
  __m256i b_bucket[NUM_POLY][SCHB_N*4];
  __m256i c_bucket[2*SCM_SIZE*4];

  mask_ar[0]=~(0UL);mask_ar[1]=~(0UL);mask_ar[2]=~(0UL);mask_ar[3]=~(0UL);
  mask_load = _mm256_loadu_si256((__m256i const *)mask_ar);

  load_values();

  for(i=0;i<KEM_K;i++)
    for(j=0;j<KEM_N/16;j++)
      sk_avx[i][j] = _mm256_loadu_si256((__m256i const *)&v->vec[i].coeffs[16*j]);

  for(i=0;i<KEM_K;i++)
    for(k=0;k<KEM_N/16;k++)
      a1_avx_combined[0][i][k] = _mm256_loadu_si256((__m256i const *)&a->vec[i].coeffs[k*16]);

  //for(j=0;j<NUM_POLY;j++)
  //  TC_eval(sk_avx[j], b_bucket[j]);

  for(i=0;i<NUM_POLY;i++) {
    toom_cook_4way_avx_n1(a1_avx_combined[0][i], b_bucket[i], c_bucket, i);
    TC_interpol(c_bucket, res_avx[0]);
  }

  for(j=0;j<KEM_N/16;j++)
    _mm256_maskstore_epi32((int *)&r->coeffs[16*j], mask_load, res_avx[0][j]);
}
