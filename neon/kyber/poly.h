#ifndef POLY_H
#define POLY_H

#include <stdint.h>
#include "params.h"

/*
 * Elements of R_q = Z_q[X]/(X^n + 1). Represents polynomial
 * coeffs[0] + X*coeffs[1] + X^2*xoeffs[2] + ... + X^{n-1}*coeffs[n-1]
 */
typedef struct{
  int16_t coeffs[KYBER_N];
} poly;

#define poly_compress KYBER_NAMESPACE(poly_compress)
void poly_compress(uint8_t r[KYBER_POLYCOMPRESSEDBYTES], const poly *a);
#define poly_decompress KYBER_NAMESPACE(poly_decompress)
void poly_decompress(poly *r, const uint8_t a[KYBER_POLYCOMPRESSEDBYTES]);

#define poly_tobytes KYBER_NAMESPACE(poly_tobytes)
void poly_tobytes(uint8_t r[KYBER_POLYBYTES], const poly *a);


#define poly_frommsg KYBER_NAMESPACE(poly_frommsg)
void poly_frommsg(poly *r, const uint8_t msg[KYBER_INDCPA_MSGBYTES]);
#define poly_tomsg KYBER_NAMESPACE(poly_tomsg)
void poly_tomsg(uint8_t msg[KYBER_INDCPA_MSGBYTES], const poly *r);

// NEON

#define neon_poly_reduce KYBER_NAMESPACE(poly_reduce)
void neon_poly_reduce(poly *c);
#define neon_poly_add_reduce KYBER_NAMESPACE(poly_add_reduce_csubq)
void neon_poly_add_reduce(poly *c, const poly *a);

#define neon_poly_sub_reduce KYBER_NAMESPACE(poly_sub_reduce_csubq)
void neon_poly_sub_reduce(poly *c, const poly *a);

#define neon_poly_add_add_reduce KYBER_NAMESPACE(poly_add_add_reduce_csubq)
void neon_poly_add_add_reduce(poly *c, const poly *a, const poly *b);

#define neon_poly_getnoise_eta1_2x KYBER_NAMESPACE(poly_getnoise_eta1_2x)
void neon_poly_getnoise_eta1_2x(poly *vec1, poly *vec2,
                                const uint8_t seed[KYBER_SYMBYTES],
                                uint8_t nonce1, uint8_t nonce2);

#define neon_poly_getnoise_eta2_2x KYBER_NAMESPACE(poly_getnoise_eta2_2x)
void neon_poly_getnoise_eta2_2x(poly *vec1, poly *vec2,
                                const uint8_t seed[KYBER_SYMBYTES],
                                uint8_t nonce1, uint8_t nonce2);

#define neon_poly_getnoise_eta2 KYBER_NAMESPACE(poly_getnoise_eta2)
void neon_poly_getnoise_eta2(poly *r,
                             const uint8_t seed[KYBER_SYMBYTES],
                             uint8_t nonce);

#define poly_frombytes KYBER_NAMESPACE(poly_frombytes)
void poly_frombytes(poly *r, const uint8_t a[KYBER_POLYBYTES]);

#define neon_poly_ntt KYBER_NAMESPACE(neon_poly_ntt)
void neon_poly_ntt(poly *r);

#define neon_poly_invntt_tomont KYBER_NAMESPACE(neon_poly_invntt_tomont)
void neon_poly_invntt_tomont(poly *r);

#endif
