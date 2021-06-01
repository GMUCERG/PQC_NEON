#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "../api.h"
#include "../SABER_params.h"
#include "../poly.h"
#include "../neon_poly_rq_mul.h"
#include "../m1cycles.h"

#define NTESTS 1000000

#define TIME(s) s = rdtsc();
// Result is clock cycles 
#define  CALC(start, stop) (stop - start) / NTESTS;


uint8_t seed[SABER_SEEDBYTES] = {0};

int main()
{
  unsigned int i;
  unsigned char pk[CRYPTO_PUBLICKEYBYTES] = {0};
  unsigned char sk[CRYPTO_SECRETKEYBYTES] = {0};
  unsigned char ct[CRYPTO_CIPHERTEXTBYTES] = {0};
  unsigned char key[CRYPTO_BYTES] = {0};
  long long start, stop;
  long long ns;
  
  uint16_t matrix[SABER_L][SABER_L][SABER_N] = {0};
  uint16_t s[SABER_L][SABER_N] = {0};
  uint16_t a[SABER_L][SABER_N] = {0};
  uint16_t b[SABER_L][SABER_N] = {0};
  uint16_t acc[SABER_N] = {0};

  // setup cycles
  setup_rdtsc();


  TIME(start);
  for(i=0;i<NTESTS;i++) {
    GenMatrix(matrix, seed);
  }
  TIME(stop);
  ns = CALC(start, stop);
  printf("GenMatrix: %lld\n", ns);

  TIME(start);
  for(i=0;i<NTESTS;i++) {
    GenSecret(s, seed);
  }
  TIME(stop);
  ns = CALC(start, stop);
  printf("GenSecret: %lld\n", ns);

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
    neonInnerProd(acc, a, b, s[0], 0);
  }
  TIME(stop);
  ns = CALC(start, stop);
  printf("neonInnerProd: %lld\n", ns);

  TIME(start);
  for(i=0;i<NTESTS;i++) {
    neonMatrixVectorMul(b, matrix, s, 1);
  }
  TIME(stop);
  ns = CALC(start, stop);
  printf("neonMatrixVectorMul: %lld\n", ns);

  return 0;
}
