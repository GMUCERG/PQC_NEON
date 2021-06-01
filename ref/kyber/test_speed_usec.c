#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "api.h"
#include "kex.h"
#include "params.h"
#include "indcpa.h"
#include "poly.h"
#include "polyvec.h"
#include "cpucycles.h"
#include "speed_print.h"

#define NTESTS 10000

uint64_t t[NTESTS];
uint8_t seed[KYBER_SYMBYTES] = {0};

int main()
{
  unsigned int i;
  unsigned char pk[CRYPTO_PUBLICKEYBYTES] = {0};
  unsigned char sk[CRYPTO_SECRETKEYBYTES] = {0};
  unsigned char ct[CRYPTO_CIPHERTEXTBYTES] = {0};
  unsigned char key[CRYPTO_BYTES] = {0};
  unsigned char kexsenda[KEX_AKE_SENDABYTES] = {0};
  unsigned char kexsendb[KEX_AKE_SENDBBYTES] = {0};
  unsigned char kexkey[KEX_SSBYTES] = {0};
  polyvec matrix[KYBER_K];
  poly ap;
  // unsigned char msg[KYBER_INDCPA_MSGBYTES] = {0};
  // poly bp;

  for(i=0;i<NTESTS;i++) {
    t[i] = cpucusec();
    gen_matrix(matrix, seed, 0);
  }
  print_results_usec("gen_a: ", t, NTESTS);

  for(i=0;i<NTESTS;i++) {
    t[i] = cpucusec();
    poly_getnoise_eta1(&ap, seed, 0);
  }
  print_results_usec("poly_getnoise_eta1: ", t, NTESTS);

  for(i=0;i<NTESTS;i++) {
    t[i] = cpucusec();
    poly_getnoise_eta2(&ap, seed, 0);
  }
  print_results_usec("poly_getnoise_eta2: ", t, NTESTS);
/* 
  for(i=0;i<NTESTS;i++) {
    t[i] = cpucusec();
    poly_tomsg(msg, &ap);
  }
  print_results_usec("poly_tomsg: ", t, NTESTS);

  for(i=0;i<NTESTS;i++) {
    t[i] = cpucusec();
    poly_frommsg(&ap, msg);
  }
  print_results_usec("poly_frommsg: ", t, NTESTS);
 */

  for(i=0;i<NTESTS;i++) {
    t[i] = cpucusec();
    poly_ntt(&ap);
  }
  print_results_usec("NTT: ", t, NTESTS);

  for(i=0;i<NTESTS;i++) {
    t[i] = cpucusec();
    poly_invntt_tomont(&ap);
  }
  print_results_usec("INVNTT: ", t, NTESTS);

  for(i=0;i<NTESTS;i++) {
    t[i] = cpucusec();
    crypto_kem_keypair(pk, sk);
  }
  print_results_usec("kyber_keypair: ", t, NTESTS);

  for(i=0;i<NTESTS;i++) {
    t[i] = cpucusec();
    crypto_kem_enc(ct, key, pk);
  }
  print_results_usec("kyber_encaps: ", t, NTESTS);

  for(i=0;i<NTESTS;i++) {
    t[i] = cpucusec();
    crypto_kem_dec(key, ct, sk);
  }
  print_results_usec("kyber_decaps: ", t, NTESTS);

  for(i=0;i<NTESTS;i++) {
    t[i] = cpucusec();
    kex_uake_initA(kexsenda, key, sk, pk);
  }
  print_results_usec("kex_uake_initA: ", t, NTESTS);

  for(i=0;i<NTESTS;i++) {
    t[i] = cpucusec();
    kex_uake_sharedB(kexsendb, kexkey, kexsenda, sk);
  }
  print_results_usec("kex_uake_sharedB: ", t, NTESTS);

  for(i=0;i<NTESTS;i++) {
    t[i] = cpucusec();
    kex_uake_sharedA(kexkey, kexsendb, key, sk);
  }
  print_results_usec("kex_uake_sharedA: ", t, NTESTS);

  for(i=0;i<NTESTS;i++) {
    t[i] = cpucusec();
    kex_ake_initA(kexsenda, key, sk, pk);
  }
  print_results_usec("kex_ake_initA: ", t, NTESTS);

  for(i=0;i<NTESTS;i++) {
    t[i] = cpucusec();
    kex_ake_sharedB(kexsendb, kexkey, kexsenda, sk, pk);
  }
  print_results_usec("kex_ake_sharedB: ", t, NTESTS);

  for(i=0;i<NTESTS;i++) {
    t[i] = cpucusec();
    kex_ake_sharedA(kexkey, kexsendb, key, sk, sk);
  }
  print_results_usec("kex_ake_sharedA: ", t, NTESTS);

  return 0;
}
