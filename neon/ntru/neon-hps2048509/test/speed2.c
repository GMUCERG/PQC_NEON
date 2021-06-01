#include "../kem.h"
#include "../params.h"
#include "../cpucycles.h"
#include "../randombytes.h"
#include "../poly.h"
#include "../sample.h"
#include <stdlib.h>
#include <stdio.h>
#include <papi.h>

static int cmp_llu(const void *a, const void *b)
{
    if (*(unsigned long long *)a < *(unsigned long long *)b)
        return -1;
    if (*(unsigned long long *)a > *(unsigned long long *)b)
        return 1;
    return 0;
}

static unsigned long long median(unsigned long long *l, size_t llen)
{
    qsort(l, llen, sizeof(unsigned long long), cmp_llu);

    if (llen % 2)
        return l[llen / 2];
    else
        return (l[llen / 2 - 1] + l[llen / 2]) / 2;
}

static double average(unsigned long long *t, size_t tlen)
{
    unsigned long long acc = 0;
    size_t i;
    for (i = 0; i < tlen; i++)
        acc += t[i];
    return ((double)acc) / (tlen);
}

static void print_results(const char *s, unsigned long long *t, size_t tlen)
{
    size_t i;
    printf("%s", s);

    unsigned long long mint = LONG_MAX;
    unsigned long long maxt = 0LL;
    for (i = 0; i < tlen - 1; i++)
    {
        t[i] = t[i + 1] - t[i];

        if (t[i] < mint)
            mint = t[i];
        if (t[i] > maxt)
            maxt = t[i];
    }
    printf("\n");
    printf("median: %llu\n", median(t, tlen));
    printf("average: %lf\n", average(t, tlen - 1));
    printf("\n");
}

void handle_error(int retval)
{
    printf("PAPI error %d: %s\n", retval, PAPI_strerror(retval));
    exit(1);
}

extern int crypto_kem_keypair(unsigned char *pk, unsigned char *sk);
extern int crypto_kem_enc(unsigned char *ct, unsigned char *ss,
                          const unsigned char *pk);
extern int crypto_kem_dec(unsigned char *ss, const unsigned char *ct,
                          const unsigned char *sk);

#define TESTS 1000

int test_kem_cca()
{

    unsigned char* pk = (unsigned char*) malloc(NTRU_PUBLICKEYBYTES);
    unsigned char* sk = (unsigned char*) malloc(NTRU_SECRETKEYBYTES);
    unsigned char* c = (unsigned char*) malloc(NTRU_CIPHERTEXTBYTES);
    unsigned char k_a[32], k_b[32];

    unsigned char entropy_input[48];
    long long unsigned t[TESTS];
    unsigned int i;

    if (PAPI_library_init(PAPI_VER_CURRENT) != PAPI_VER_CURRENT)
        exit(1);

    for (i = 0; i < 48; i++)
    {
        entropy_input[i] = i;
    }
    randombytes(entropy_input, sizeof(entropy_input));
    /* =================================== */
    for (i = 0; i < TESTS; i++)
    {
        crypto_kem_keypair(pk, sk);
        t[i] = PAPI_get_real_usec();
    }
    print_results("Keygen:           ", t, TESTS);

    /* =================================== */
    for (i = 0; i < TESTS; i++)
    {
        crypto_kem_enc(c, k_a, pk);
        t[i] = PAPI_get_real_usec();
    }
    print_results("Encap:           ", t, TESTS);

    /* =================================== */
    for (i = 0; i < TESTS; i++)
    {
        crypto_kem_dec(k_b, c, sk);
        t[i] = PAPI_get_real_usec();
    }
    print_results("Decap:           ", t, TESTS);

    /* =================================== */
    return 0;
}

int main()
{
    test_kem_cca();
    return 0;
}
