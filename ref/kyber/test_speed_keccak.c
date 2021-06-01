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
#include "ntt.h"
#include "symmetric.h"
#include "papi.h"

#define NTESTS 1000000

uint8_t seed[KYBER_SYMBYTES] = {0};

int main()
{
    unsigned int i;
    uint8_t buf1eta2[KYBER_ETA2 * KYBER_N / 4],
        buf2eta2[KYBER_ETA2 * KYBER_N / 4];
    uint8_t buf1eta1[KYBER_ETA1 * KYBER_N / 4],
        buf2eta1[KYBER_ETA1 * KYBER_N / 4];
    polyvec matrix[KYBER_K];
    poly ap, bp;
    int retval;
    xof_state state1, state2;

#define GEN_MATRIX_NBLOCKS ((12 * KYBER_N / 8 * (1 << 12) / KYBER_Q + XOF_BLOCKBYTES) / XOF_BLOCKBYTES)

    uint8_t buf0[GEN_MATRIX_NBLOCKS * XOF_BLOCKBYTES + 2],
        buf1[GEN_MATRIX_NBLOCKS * XOF_BLOCKBYTES + 2];

    printf("NTESTS: %d\n", NTESTS);
    long_long start, end;

    PAPI_hl_region_begin("gen_matrix");
    for (i = 0; i < NTESTS; i++)
    {
        gen_matrix(matrix, seed, 0);
    }
    PAPI_hl_region_end("gen_matrix");

    PAPI_hl_region_begin("eta1");
    for (i = 0; i < NTESTS; i++)
    {
        poly_getnoise_eta1(&ap, seed, 1);
        poly_getnoise_eta1(&bp, seed, 0);
    }
    PAPI_hl_region_end("eta1");

    PAPI_hl_region_begin("eta2");
    for (i = 0; i < NTESTS; i++)
    {
        poly_getnoise_eta2(&ap, seed, 0);
        poly_getnoise_eta2(&bp, seed, 1);
    }
    PAPI_hl_region_end("eta2");

    PAPI_hl_region_begin("prf");
    for (i = 0; i < NTESTS; i++)
    {
        prf(buf1eta1, sizeof(buf1eta1), seed, 0);
        prf(buf2eta1, sizeof(buf2eta1), seed, 0);
    }
    PAPI_hl_region_end("prf");

    PAPI_hl_region_begin("prf2");
    for (i = 0; i < NTESTS; i++)
    {
        prf(buf1eta2, sizeof(buf1eta2), seed, 0);
        prf(buf2eta2, sizeof(buf2eta2), seed, 0);
    }
    PAPI_hl_region_end("prf2");

    PAPI_hl_region_begin("abs");
    for (i = 0; i < NTESTS; i++)
    {
        kyber_shake128_absorb(&state1, seed, 0, 1);
        kyber_shake128_absorb(&state2, seed, 0, 1);
        xof_squeezeblocks(buf0, GEN_MATRIX_NBLOCKS, &state1);
        xof_squeezeblocks(buf1, GEN_MATRIX_NBLOCKS, &state2);
    }
    PAPI_hl_region_end("abs");

    return 0;
}
