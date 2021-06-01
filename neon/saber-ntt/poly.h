#ifndef POLY_H
#define POLY_H

#include <stdint.h>
#include "params.h"

typedef struct
{
    __attribute__((aligned(32)))
    int16_t coeffs[POLY_N];
} poly;

typedef struct
{
    __attribute__((aligned(32)))
    int16_t coeffs[NTT_N];
} nttpoly;


#define neon_poly_crt POLYMUL_NAMESPACE(_neon_poly_crt)
void neon_poly_crt(poly *r, const poly *a, const poly *b);



#endif
