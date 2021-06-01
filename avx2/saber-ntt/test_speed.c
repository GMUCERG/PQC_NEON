#include <stdio.h>
#include <string.h>
#include "params.h"
#include "randombytes.h"
#include "cpucycles.h"
#include "speed_print.h"
#ifdef KEM_K
#include "polyvec.h"
#endif
#include "poly.h"
#include "consts.h"
// #include "printf.h"
#include <time.h>

#define NTESTS 10000000

#define TIME(s) s = cpucycles();
// Result is nanosecond per call 
#define  CALC(start, stop) (stop - start) / NTESTS;


int main(void) {
  int k;
  // uint64_t tsc[100];
  poly a, b, c;
  nttpoly ahat, bhat, chat;
  long long start, stop;
  long long ns;

  memset(&a,0,sizeof(poly));
  memset(&b,0,sizeof(poly));

#ifdef KEM_K
  polyvec mat[KEM_K];
  polyvec s, t;
  nttpolyvec mhat0[KEM_K], mhat1[KEM_K], shat0, shat1, that0, that1;
  TIME(start);
  for(k=0;k<NTESTS;k++) {
    // tsc[k] = cpucycles();
    saber_matrix_vector_mul(&t,mat,&s);
  }
  TIME(stop);
  ns = CALC(start, stop);
  printf("saber_matrix_vector_mul %llu\n", ns);
  
  TIME(start)
  for(k=0;k<NTESTS;k++) {
    // tsc[k] = cpucycles();
    saber_iprod(&a,&t,&s);
  }
  TIME(stop);
  ns = CALC(start, stop);
  printf("saber_iprod: %llu\n", ns);
  // print_results("saber_iprod:",tsc,100);
#else
  TIME(start);
  for(k=0;k<NTESTS;k++) {
    // tsc[k] = cpucycles();
    orig_poly_mul(&c,&a,&b);
  }
  TIME(stop);
  ns = CALC(start, stop);
  printf("ntru_poly_mul: %llu\n", ns);
  // print_results("ntru_poly_mul:",tsc,100);
#endif

  TIME(start);
  for(k=0;k<NTESTS;k++) {
    // tsc[k] = cpucycles();
    poly_ntt(&ahat,&a,PDATA0);
  }
  TIME(stop);
  ns = CALC(start, stop);
  printf("poly_ntt: %llu\n", ns);
  // print_results("poly_ntt:",tsc,100);

  TIME(start);
  for(k=0;k<NTESTS;k++) {
    // tsc[k] = cpucycles();
    poly_invntt_tomont(&ahat,&ahat,PDATA0);
  }
  TIME(stop);
  ns = CALC(start, stop);
  printf("poly_invntt_tomont: %llu\n", ns);
  // print_results("poly_invntt_tomont:",tsc,100);

  TIME(start);
  for(k=0;k<NTESTS;k++) {
    // tsc[k] = cpucycles();
    poly_basemul_montgomery(&ahat,&ahat,&bhat,PDATA0);
  }
  TIME(stop);
  ns = CALC(start, stop);
  printf("poly_basemul_montgomery: %llu\n", ns);
  // print_results("poly_basemul_montgomery:",tsc,100);

#ifdef KEM_K
  TIME(start);
  for(k=0;k<NTESTS;k++) {
    // tsc[k] = cpucycles();
    polyvec_basemul_acc_montgomery(&ahat,&shat0,&that0,PDATA0);
  }
  TIME(stop);
  ns = CALC(start, stop);
  printf("polyvec_basemul_acc_montgomery: %llu\n", ns);
  // print_results("polyvec_basemul_acc_montgomery:",tsc,100);
#endif

  TIME(start);
  for(k=0;k<NTESTS;k++) {
    // tsc[k] = cpucycles();
    poly_crt(&a,&ahat,&bhat);
  }
  TIME(stop);
  ns = CALC(start, stop);
  printf("poly_crt: %llu\n", ns);
  // print_results("poly_crt:",tsc,100);

  TIME(start);
  for(k=0;k<NTESTS;k++) {
    // tsc[k] = cpucycles();
    poly_ntt(&ahat,&a,PDATA0);
    poly_ntt(&bhat,&b,PDATA0);
    poly_basemul_montgomery(&ahat,&ahat,&bhat,PDATA0);
    poly_invntt_tomont(&ahat,&ahat,PDATA0);
    poly_ntt(&bhat,&a,PDATA1);
    poly_ntt(&chat,&b,PDATA1);
    poly_basemul_montgomery(&bhat,&bhat,&chat,PDATA1);
    poly_invntt_tomont(&bhat,&bhat,PDATA1);
    poly_crt(&c,&ahat,&bhat);
  }
  TIME(stop);
  ns = CALC(start, stop);
  printf("ntt-based poly_mul: %llu\n", ns);
  // print_results("ntt-based poly_mul:",tsc,100);

#ifdef KEM_K
  TIME(start);
  for(k=0;k<NTESTS;k++) {
    // tsc[k] = cpucycles();
    int i;
    for(i=0;i<KEM_K;i++)
      polyvec_ntt(&mhat1[i],&mat[i],PDATA1);
    polyvec_ntt(&shat1,&s,PDATA1);
    for(i=0;i<KEM_K;i++)
      polyvec_ntt(&mhat0[i],&mat[i],PDATA0);
    polyvec_ntt(&shat0,&s,PDATA0);

    for(i=0;i<KEM_K;i++)
      polyvec_basemul_acc_montgomery(&that0.vec[i],&mhat0[i],&shat0,PDATA0);
    for(i=0;i<KEM_K;i++)
      polyvec_basemul_acc_montgomery(&that1.vec[i],&mhat1[i],&shat1,PDATA1);

    polyvec_invntt_tomont(&that1,&that1,PDATA1);
    polyvec_invntt_tomont(&that0,&that0,PDATA0);
    polyvec_crt(&t,&that0,&that1);
  }
  TIME(stop);
  ns = CALC(start, stop);
  printf("polyvec_matrix_vector_mul: %llu\n", ns);
  // print_results("polyvec_matrix_vector_mul:",tsc,100);

  TIME(start);
  for(k=0;k<NTESTS;k++) {
    // tsc[k] = cpucycles();
    polyvec_iprod(&a,&t,&s);
  }
  TIME(stop);
  ns = CALC(start, stop);
  printf("polyvec_iprod: %llu\n", ns);
  // print_results("polyvec_iprod:",tsc,100);
#endif

  return 0;
}
