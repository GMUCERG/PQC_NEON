#ifndef NEON_NTT_H
#define NEON_NTT_H 

#include <arm_neon.h>
#include "poly.h"

void neon_ntt(nttpoly *out, const poly *in, const int16_t *pdata);

void neon_invntt(poly *out, const nttpoly *in, const int16_t *pdata);

#endif
