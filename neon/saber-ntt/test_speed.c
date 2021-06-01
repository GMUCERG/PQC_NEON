#include <stdint.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include "params.h"
#include "neon_ntt.h"
#include "poly.h"
#include "polyvec.h"
#include "consts256.h"
#include "m1cycles.h"

#define NTESTS 1000000
// #define NTESTS 1000

#define TIME(s) s = rdtsc();
// Result is clock cycles
#define CALC(start, stop) (stop - start) / NTESTS;

int main(void)
{
    int i, j, n;
    polyvec a[KEM_K];
    polyvec t, s;
    poly r;
    nttpoly z;
    long long start, stop;
    long long ns;

    for (i = 0; i < KEM_K; i++)
    {
        for (n = 0; n < KEM_N; n++)
        {
            t.vec[i].coeffs[n] = (i*0x4131 + n) % KEM_Q;
            s.vec[i].coeffs[n] = (i*0x4131 + n) % KEM_Q;
            r.coeffs[n] = (i*0x4131 + n) % KEM_Q;
        }


        for (j = 0; j < KEM_K; j++)
        {
            for (n =0; n < KEM_N; n++)
            {
                a[i].vec[j].coeffs[n] = (i*0x31 +j*0x1337 + n) % KEM_Q;
            }
        }
    }


    setup_rdtsc();

    TIME(start);
    for (i = 0; i < NTESTS; i++)
    {
        neon_ntt(&z, &r, PDATA10753);
    }
    TIME(stop);
    ns = CALC(start, stop);
    printf("neon_ntt: %lld\n", ns);

    TIME(start);
    for (i = 0; i < NTESTS; i++)
    {
        neon_invntt(&r, &z, PDATA10753);
    }
    TIME(stop);
    ns = CALC(start, stop);
    printf("neon_invntt: %lld\n", ns);

    TIME(start);
    for (i = 0; i < NTESTS; i++)
    {
        polyvec_iprod(&r, &t, &s);
    }
    TIME(stop);
    ns = CALC(start, stop);
    printf("polyvec_iprod: %lld\n", ns);

    TIME(start);
    for (i = 0; i < NTESTS; i++)
    {
        polyvec_matrix_vector_mul(&t, a, &s, 0);
    }
    TIME(stop);
    ns = CALC(start, stop);
    printf("polyvec_matrix_vector_mul: %lld\n", ns);

    return 0;
}
