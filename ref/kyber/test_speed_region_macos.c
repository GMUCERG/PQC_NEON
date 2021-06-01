#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "kem.h"
#include "kex.h"
#include "params.h"
#include "indcpa.h"
#include "poly.h"
#include "polyvec.h"
#include "m1cycles.h"
#include "ntt.h"

#define NTESTS 1000000

uint8_t seed[KYBER_SYMBYTES] = {0};

#define TIME(s) s = rdtsc();
// Result is clock cycles 
#define  CALC(start, stop) (stop - start) / NTESTS;


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
  unsigned char pk[CRYPTO_PUBLICKEYBYTES] = {0};
  unsigned char sk[CRYPTO_SECRETKEYBYTES] = {0};
  unsigned char ct[CRYPTO_CIPHERTEXTBYTES] = {0};
  unsigned char key[CRYPTO_BYTES] = {0};
//   unsigned char kexsenda[KEX_AKE_SENDABYTES] = {0};
//   unsigned char kexsendb[KEX_AKE_SENDBBYTES] = {0};
//   unsigned char kexkey[KEX_SSBYTES] = {0};
  unsigned char msg[KYBER_INDCPA_MSGBYTES] = {0};
  polyvec matrix[KYBER_K];
  poly ap;
  polyvec sp, b;

//   struct timespec start, stop;
  long long ns;
  long long start, stop;


// Init performance counter 
  setup_rdtsc();
// End 


  TIME(start);
  for(i=0;i<NTESTS;i++) {
    gen_matrix(matrix, seed, 0);
  }
  TIME(stop);
  ns = CALC(start, stop);
  printf("gen_matrix: %lld\n", ns);

  TIME(start);
  for(i=0;i<NTESTS;i++) {
    poly_getnoise_eta1(&ap, seed, 1);
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

  TIME(start);
  for(i=0;i<NTESTS;i++) {
    poly_tomsg(msg, &ap);
  }
  TIME(stop);
  ns = CALC(start, stop);
  printf("poly_tomsg: %lld\n", ns);

  TIME(start);
  for(i=0;i<NTESTS;i++) {
    poly_frommsg(&ap, msg);
  }
  TIME(stop);
  ns = CALC(start, stop);
  printf("poly_frommsg: %lld\n", ns);


  TIME(start);
  for(i=0;i<NTESTS;i++) {
    ntt(ap.coeffs);
  }
  TIME(stop);
  ns = CALC(start, stop);
  printf("ref_ntt: %lld\n", ns);

  TIME(start);
  for(i=0;i<NTESTS;i++) {
    invntt(ap.coeffs);
  }
  TIME(stop);
  ns = CALC(start, stop);
  printf("ref_invntt: %lld\n", ns);

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



  return 0;
}
