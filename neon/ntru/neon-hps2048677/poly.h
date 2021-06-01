#ifndef POLY_H
#define POLY_H

#include "params.h"

#include <stddef.h>
#include <stdint.h>

#define MODQ(X) ((X) & (NTRU_Q-1))
#define NTRU_N_32 704
#define NTRU_N_PAD 720

typedef struct{
  uint16_t coeffs[NTRU_N_PAD];
} poly;

#define poly_mod_3_Phi_n CRYPTO_NAMESPACE(poly_mod_3_Phi_n)
#define poly_mod_q_Phi_n CRYPTO_NAMESPACE(poly_mod_q_Phi_n)
void poly_mod_3_Phi_n(poly *r);
void poly_mod_q_Phi_n(poly *r);

#define poly_Sq_tobytes CRYPTO_NAMESPACE(poly_Sq_tobytes)
#define poly_Sq_frombytes CRYPTO_NAMESPACE(poly_Sq_frombytes)
void poly_Sq_tobytes(unsigned char *r, const poly *a);
void poly_Sq_frombytes(poly *r, const unsigned char *a);

#define poly_Rq_sum_zero_tobytes CRYPTO_NAMESPACE(poly_Rq_sum_zero_tobytes)
#define poly_Rq_sum_zero_frombytes CRYPTO_NAMESPACE(poly_Rq_sum_zero_frombytes)
void poly_Rq_sum_zero_tobytes(unsigned char *r, const poly *a);
void poly_Rq_sum_zero_frombytes(poly *r, const unsigned char *a);

#define poly_S3_tobytes CRYPTO_NAMESPACE(poly_S3_tobytes)
#define poly_S3_frombytes CRYPTO_NAMESPACE(poly_S3_frombytes)
void poly_S3_tobytes(unsigned char msg[NTRU_PACK_TRINARY_BYTES], const poly *a);
void poly_S3_frombytes(poly *r, const unsigned char msg[NTRU_PACK_TRINARY_BYTES]);

#define poly_Sq_mul CRYPTO_NAMESPACE(poly_Sq_mul)
#define poly_Rq_mul CRYPTO_NAMESPACE(poly_Rq_mul)
#define poly_S3_mul CRYPTO_NAMESPACE(poly_S3_mul)
#define poly_lift CRYPTO_NAMESPACE(poly_lift)
#define poly_lift_add CRYPTO_NAMESPACE(poly_lift_add)
#define poly_lift_sub CRYPTO_NAMESPACE(poly_lift_sub)
#define poly_Rq_to_S3 CRYPTO_NAMESPACE(poly_Rq_to_S3)
void poly_Sq_mul(poly *r, poly *a, poly *b);
void poly_Rq_mul(poly *r, poly *a, poly *b);
void poly_S3_mul(poly *r, poly *a, poly *b);
void poly_lift(poly *r, const poly *a);
void poly_lift_add(poly *ct, const poly *a);
void poly_lift_sub(poly *b, const poly *c, const poly *a);
void poly_Rq_to_S3(poly *r, const poly *a);

#define poly_R2_inv CRYPTO_NAMESPACE(poly_R2_inv)
#define poly_Rq_inv CRYPTO_NAMESPACE(poly_Rq_inv)
#define poly_S3_inv CRYPTO_NAMESPACE(poly_S3_inv)
void poly_R2_inv(poly *r, const poly *a);
void poly_Rq_inv(poly *r, const poly *a);
void poly_S3_inv(poly *r, const poly *a);

#define poly_Z3_to_Zq CRYPTO_NAMESPACE(poly_Z3_to_Zq)
#define poly_trinary_Zq_to_Z3 CRYPTO_NAMESPACE(poly_trinary_Zq_to_Z3)
void poly_Z3_to_Zq(poly *r);
void poly_trinary_Zq_to_Z3(poly *r);

#define polyhps_mul3 CRYPTO_NAMESPACE(polyhps_mul3)
#define polyhrss_mul3 CRYPTO_NAMESPACE(polyhrss_mul3)
void polyhps_mul3(poly *g);
void polyhrss_mul3(poly *g);
#endif
