#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "kem.h"
#include "kex.h"
#include "params.h"
#include "indcpa.h"
#include "polyvec.h"
#include "poly.h"
#include "randombytes.h"
#include "cpucycles.h"
#include "speed_print.h"

#define NTESTS 1000000

uint8_t seed[KYBER_SYMBYTES] = {0};

/* Dummy randombytes for speed tests that simulates a fast randombytes implementation
 * as in SUPERCOP so that we get comparable cycle counts */
void randombytes(__attribute__((unused)) uint8_t *r, __attribute__((unused)) size_t len) {
  return;
}

#define TIME(s) s = cpucycles();
// Result is nanosecond per call 
#define  CALC(start, stop) (stop - start)/NTESTS;


static 
void VectorVectorMul(poly *mp, polyvec *b, polyvec *skpv)
{
  polyvec_ntt(b);
  polyvec_basemul_acc_montgomery(mp, skpv, b);
  poly_invntt_tomont(mp);
}


static 
void MatrixVectorMul(polyvec at[KYBER_K], polyvec *sp, polyvec *b)
{
  polyvec_ntt(sp);

  // matrix-vector multiplication
  for(int i=0;i<KYBER_K;i++)
    polyvec_basemul_acc_montgomery(&b->vec[i], &at[i], sp);

  polyvec_invntt_tomont(b);
}



int main()
{
  unsigned int i;
  uint8_t pk[CRYPTO_PUBLICKEYBYTES];
  uint8_t sk[CRYPTO_SECRETKEYBYTES];
  uint8_t ct[CRYPTO_CIPHERTEXTBYTES];
  uint8_t key[CRYPTO_BYTES];
  uint8_t kexsenda[KEX_AKE_SENDABYTES];
  uint8_t kexsendb[KEX_AKE_SENDBBYTES];
  uint8_t kexkey[KEX_SSBYTES];
  polyvec matrix[KYBER_K];
  poly ap;
  polyvec sp, b;
  long long start, stop;
  long long ns;

#ifndef KYBER_90S
  poly bp, cp, dp;
#endif

  TIME(start);
  for(i=0;i<NTESTS;i++) {
    gen_matrix(matrix, seed, 0);
  }
  TIME(stop);
  ns = CALC(start, stop);
  printf("gen_matrix: %lld\n", ns);


  TIME(start);
  for(i=0;i<NTESTS;i++) {
    poly_getnoise_eta1(&ap, seed, 0);
  }
  TIME(stop);
  ns = CALC(start, stop);
  printf("poly_getnoise_eta1: %lld\n", ns);

  TIME(start);
  for(i=0;i<NTESTS;i++) {
    poly_getnoise_eta2(&ap, seed, 0);
  }
  TIME(stop);
  ns = CALC(start, stop);
  printf("poly_getnoise_eta2: %lld\n", ns);

#ifndef KYBER_90S
  TIME(start);
  for(i=0;i<NTESTS;i++) {
    poly_getnoise_eta1_4x(&ap, &bp, &cp, &dp, seed, 0, 1, 2, 3);
  }
  TIME(stop);
  ns = CALC(start, stop);
  printf("poly_getnoise_eta1_4x: %lld\n", ns);
#endif

  TIME(start);
  for(i=0;i<NTESTS;i++) {
    poly_ntt(&ap);
  }
  TIME(stop);
  ns = CALC(start, stop);
  printf("poly_ntt: %lld\n", ns);

  TIME(start);
  for(i=0;i<NTESTS;i++) {
    poly_invntt_tomont(&ap);
  }
  TIME(stop);
  ns = CALC(start, stop);
  printf("poly_invntt_tomont: %lld\n", ns);

  TIME(start);
  for(i=0;i<NTESTS;i++) {
    polyvec_basemul_acc_montgomery(&ap, &matrix[0], &matrix[1]);
  }
  TIME(stop);
  ns = CALC(start, stop);
  printf("polyvec_basemul_acc_montgomery: %lld\n", ns);

  TIME(start);
  for(i=0;i<NTESTS;i++) {
    poly_tomsg(ct,&ap);
  }
  TIME(stop);
  ns = CALC(start, stop);
  printf("poly_tomsg: %lld\n", ns);
  
  TIME(start);
  for(i=0;i<NTESTS;i++) {
    poly_frommsg(&ap,ct);
  }
  TIME(stop);
  ns = CALC(start, stop);
  printf("poly_frommsg: %lld\n", ns);

  TIME(start);
  for(i=0;i<NTESTS;i++) {
    poly_compress(ct,&ap);
  }
  TIME(stop);
  ns = CALC(start, stop);
  printf("poly_compress: %lld\n", ns);
  
  TIME(start);
  for(i=0;i<NTESTS;i++) {
    poly_decompress(&ap,ct);
  }
  TIME(stop);
  ns = CALC(start, stop);
  printf("poly_decompress: %lld\n", ns);

  TIME(start);
  for(i=0;i<NTESTS;i++) {
    polyvec_compress(ct,&matrix[0]);
  }
  TIME(stop);
  ns = CALC(start, stop);
  printf("polyvec_compress: %lld\n", ns);

  TIME(start);
  for(i=0;i<NTESTS;i++) {
    polyvec_decompress(&matrix[0],ct);
  }
  TIME(stop);
  ns = CALC(start, stop);
  printf("polyvec_decompress: %lld\n", ns);

  TIME(start);
  for(i=0;i<NTESTS;i++) {
    indcpa_keypair(pk, sk);
  }
  TIME(stop);
  ns = CALC(start, stop);
  printf("indcpa_keypair: %lld\n", ns);
  
  TIME(start);
  for(i=0;i<NTESTS;i++) {
    indcpa_enc(ct, key, pk, seed);
  }
  TIME(stop);
  ns = CALC(start, stop);
  printf("indcpa_enc: %lld\n", ns);

  TIME(start);
  for(i=0;i<NTESTS;i++) {
    indcpa_dec(key, ct, sk);
  }
  TIME(stop);
  ns = CALC(start, stop);
  printf("indcpa_dec: %lld\n", ns);

  TIME(start);
  for(i=0;i<NTESTS;i++) {
    crypto_kem_keypair(pk, sk);
  }
  TIME(stop);
  ns = CALC(start, stop);
  printf("crypto_kem_keypair: %lld\n", ns);
  
  TIME(start);
  for(i=0;i<NTESTS;i++) {
    crypto_kem_enc(ct, key, pk);
  }
  TIME(stop);
  ns = CALC(start, stop);
  printf("crypto_kem_enc: %lld\n", ns);
  
  TIME(start);
  for(i=0;i<NTESTS;i++) {
    crypto_kem_dec(key, ct, sk);
  }
  TIME(stop);
  ns = CALC(start, stop);
  printf("crypto_kem_dec: %lld\n", ns);

  TIME(start);
  for(i=0;i<NTESTS;i++) {
    VectorVectorMul(&ap, &sp, &b);
  }
  TIME(stop);
  ns = CALC(start, stop);
  printf("VectorVectorMul: %lld\n", ns);

  
  TIME(start);
  for(i=0;i<NTESTS;i++) {
    MatrixVectorMul(matrix, &sp, &b);
  }
  TIME(stop);
  ns = CALC(start, stop);
  printf("MatrixVectorMul: %lld\n", ns);

/* 
  for(i=0;i<NTESTS;i++) {
    t[i] = cpucycles();
    kex_uake_initA(kexsenda, key, sk, pk);
  }
  

  for(i=0;i<NTESTS;i++) {
    t[i] = cpucycles();
    kex_uake_sharedB(kexsendb, kexkey, kexsenda, sk);
  }
  

  for(i=0;i<NTESTS;i++) {
    t[i] = cpucycles();
    kex_uake_sharedA(kexkey, kexsendb, key, sk);
  }
  

  for(i=0;i<NTESTS;i++) {
    t[i] = cpucycles();
    kex_ake_initA(kexsenda, key, sk, pk);
  }
  

  for(i=0;i<NTESTS;i++) {
    t[i] = cpucycles();
    kex_ake_sharedB(kexsendb, kexkey, kexsenda, sk, pk);
  }
  

  for(i=0;i<NTESTS;i++) {
    t[i] = cpucycles();
    kex_ake_sharedA(kexkey, kexsendb, key, sk, sk);
  }
  
 */
  return 0;
}
