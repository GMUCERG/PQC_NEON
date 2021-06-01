#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "../SABER_params.h"
#include "../api.h"
#include "../rng.h"

#include <papi.h>

static int cmp_llu(const void *a, const void *b) {
  if (*(unsigned long long *)a < *(unsigned long long *)b)
    return -1;
  if (*(unsigned long long *)a > *(unsigned long long *)b)
    return 1;
  return 0;
}

static unsigned long long median(unsigned long long *l, size_t llen) {
  qsort(l, llen, sizeof(unsigned long long), cmp_llu);

  if (llen % 2)
    return l[llen / 2];
  else
    return (l[llen / 2 - 1] + l[llen / 2]) / 2;
}

static double average(unsigned long long *t, size_t tlen) {
  unsigned long long acc = 0;
  size_t i;
  for (i = 0; i < tlen; i++)
    acc += t[i];
  return ((double)acc) / (tlen);
}

static void print_results(const char *s, unsigned long long *t, size_t tlen) {
  size_t i;
  printf("%s", s);

  unsigned long long mint = LONG_MAX;
  unsigned long long maxt = 0LL;
  for (i = 0; i < tlen - 1; i++) {
    t[i] = t[i + 1] - t[i];

    if (t[i] < mint)
      mint = t[i];
    if (t[i] > maxt)
      maxt = t[i];
  }
  printf("\n");
  printf("median: %'llu\n", median(t, tlen));
  printf("average: %'lf\n", average(t, tlen - 1));
  printf("\n");
}

void handle_error(int retval) {
  printf("PAPI error %d: %s\n", retval, PAPI_strerror(retval));
  exit(1);
}

#define NTESTS 100000

int test_kem_cca() {

  uint8_t pk[SABER_PUBLICKEYBYTES];
  uint8_t sk[SABER_SECRETKEYBYTES];
  uint8_t c[SABER_BYTES_CCA_DEC];
  uint8_t k_a[SABER_KEYBYTES], k_b[SABER_KEYBYTES];

  unsigned char entropy_input[48];

  uint64_t i, repeat;
  repeat = NTESTS;

  unsigned long long t[NTESTS];

  for (i = 0; i < NTESTS; i++);


  if (PAPI_library_init(PAPI_VER_CURRENT) != PAPI_VER_CURRENT)
    exit(1);

  randombytes_init(entropy_input, NULL, 256);
  /* =================================== */
  for (i = 0; i < repeat; i++) {
    crypto_kem_keypair(pk, sk);
    t[i] = PAPI_get_real_usec();
  }
  print_results("Keygen:           ", t, NTESTS);

  /* =================================== */
  for (i = 0; i < repeat; i++) {
    crypto_kem_enc(c, k_a, pk);
    t[i] = PAPI_get_real_usec();
  }
  print_results("Encap:           ", t, NTESTS);

  /* =================================== */
  for (i = 0; i < repeat; i++) {
    crypto_kem_dec(k_b, c, sk);
    t[i] = PAPI_get_real_usec();
  }
  print_results("Decap:           ", t, NTESTS);

  /* =================================== */
  return 0;
}

int main() {
  test_kem_cca();
  return 0;
}
