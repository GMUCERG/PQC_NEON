#include <arm_neon.h>
#include "params.h"
#include "consts256.h"
#include "polyvec.h"
#include "neon_ntt.h"

/*************************************************/
// Load int16x8x4_t c <= ptr*
#define vloadx4(c, ptr) c = vld1q_s16_x4(ptr);

// Store *ptr <= c
#define vstorex4(ptr, c) vst1q_s16_x4(ptr, c);

// c (int16x8) = a - b (int16x8)
#define vsub8(c, a, b) c = vsubq_s16(a, b);

// c (int16x8) = a + b (int16x8)
#define vadd8(c, a, b) c = vaddq_s16(a, b);

// c (int16x8) = a & b (int16x8)
#define vand8(c, a, b) c = vandq_s16(a, b);

#define fqmul(out, in, zeta, t)                                                                           \
    t.val[0] = (int16x8_t)vmull_s16(vget_low_s16(in), vget_low_s16(zeta));                                \
    t.val[1] = (int16x8_t)vmull_high_s16(in, zeta);                                                       \
    t.val[2] = vuzp1q_s16(t.val[0], t.val[1]);                                     /* a_L  */             \
    t.val[3] = vuzp2q_s16(t.val[0], t.val[1]);                                     /* a_H  */             \
    t.val[0] = vmulq_s16(t.val[2], neon_pinv);                                     /* a_L = a_L * QINV */ \
    t.val[1] = (int16x8_t)vmull_s16(vget_low_s16(t.val[0]), vget_low_s16(neon_p)); /* t_L = a_L * Q */    \
    t.val[2] = (int16x8_t)vmull_high_s16(t.val[0], neon_p);                        /* t_H = a_L * Q*/     \
    t.val[0] = vuzp2q_s16(t.val[1], t.val[2]);                                     /* t_H */              \
    out = vsubq_s16(t.val[3], t.val[0]);          
/*************************************************/

void polyvec_basemul_acc_montgomery(nttpoly *r, const nttpolyvec *a, const nttpolyvec *b, const int16_t *pdata)
{
    int16x8x4_t va, vb, vr, vt, t; 
    int16x8_t neon_p, neon_pinv, neon_f;

    neon_p = vld1q_s16(&pdata[_8XP]);
    neon_pinv = vld1q_s16(&pdata[_8XPINV]);
    neon_f = vld1q_s16(&pdata[_8XF]);

    // Total SIMD registers: 24
    for (int i = 0; i < KEM_N; i+=32)
    {
        vloadx4(va, &a->vec[0].coeffs[i]);
        vloadx4(vb, &b->vec[0].coeffs[i]);

        fqmul(vr.val[0], va.val[0], vb.val[0], t);
        fqmul(vr.val[1], va.val[1], vb.val[1], t);
        fqmul(vr.val[2], va.val[2], vb.val[2], t);
        fqmul(vr.val[3], va.val[3], vb.val[3], t);

        vloadx4(va, &a->vec[1].coeffs[i]);
        vloadx4(vb, &b->vec[1].coeffs[i]);

        fqmul(vt.val[0], va.val[0], vb.val[0], t);
        fqmul(vt.val[1], va.val[1], vb.val[1], t);
        fqmul(vt.val[2], va.val[2], vb.val[2], t);
        fqmul(vt.val[3], va.val[3], vb.val[3], t);

        vadd8(vr.val[0], vr.val[0], vt.val[0]);
        vadd8(vr.val[1], vr.val[1], vt.val[1]);
        vadd8(vr.val[2], vr.val[2], vt.val[2]);
        vadd8(vr.val[3], vr.val[3], vt.val[3]);

#if KEM_K > 2 
        vloadx4(va, &a->vec[2].coeffs[i]);
        vloadx4(vb, &b->vec[2].coeffs[i]);

        fqmul(vt.val[0], va.val[0], vb.val[0], t);
        fqmul(vt.val[1], va.val[1], vb.val[1], t);
        fqmul(vt.val[2], va.val[2], vb.val[2], t);
        fqmul(vt.val[3], va.val[3], vb.val[3], t);

        vadd8(vr.val[0], vr.val[0], vt.val[0]);
        vadd8(vr.val[1], vr.val[1], vt.val[1]);
        vadd8(vr.val[2], vr.val[2], vt.val[2]);
        vadd8(vr.val[3], vr.val[3], vt.val[3]);
#endif 

#if KEM_K > 3 
        vloadx4(va, &a->vec[3].coeffs[i]);
        vloadx4(vb, &b->vec[3].coeffs[i]);

        fqmul(vt.val[0], va.val[0], vb.val[0], t);
        fqmul(vt.val[1], va.val[1], vb.val[1], t);
        fqmul(vt.val[2], va.val[2], vb.val[2], t);
        fqmul(vt.val[3], va.val[3], vb.val[3], t);

        vadd8(vr.val[0], vr.val[0], vt.val[0]);
        vadd8(vr.val[1], vr.val[1], vt.val[1]);
        vadd8(vr.val[2], vr.val[2], vt.val[2]);
        vadd8(vr.val[3], vr.val[3], vt.val[3]);
#endif

        fqmul(vr.val[0], vr.val[0], neon_f, t);
        fqmul(vr.val[1], vr.val[1], neon_f, t);
        fqmul(vr.val[2], vr.val[2], neon_f, t);
        fqmul(vr.val[3], vr.val[3], neon_f, t);

        vstorex4(&r->coeffs[i], vr);
    }

}


void polyvec_ntt(nttpolyvec *r, const polyvec *a, const int16_t *pdata) {
  unsigned int i;
  for(i=0;i<KEM_K;i++)
    neon_ntt(&r->vec[i], &a->vec[i], pdata);
}

void polyvec_invntt_tomont(polyvec *r, const nttpolyvec *a, const int16_t *pdata) {
  unsigned int i;
  for(i=0;i<KEM_K;i++)
    neon_invntt(&r->vec[i], &a->vec[i], pdata);
}

void polyvec_crt(polyvec *r, const polyvec *a, const polyvec *b) {
  unsigned int i;
  for(i=0;i<KEM_K;i++)
    neon_poly_crt(&r->vec[i], &a->vec[i], &b->vec[i]);
}

void polyvec_matrix_vector_mul(polyvec *t, const polyvec a[KEM_K], const polyvec *s, int transpose) {
  unsigned int i, j;
  nttpolyvec shat, ahat, t0, t1;
  polyvec z0, z1;

  polyvec_ntt(&shat,s,PDATA7681);
  for(i=0;i<KEM_K;i++) {
    for(j=0;j<KEM_K;j++) {
      if(transpose)
        neon_ntt(&ahat.vec[j],&a[j].vec[i],PDATA7681);
      else
        neon_ntt(&ahat.vec[j],&a[i].vec[j],PDATA7681);
    }
    polyvec_basemul_acc_montgomery(&t0.vec[i],&ahat,&shat,PDATA7681);
  }

  polyvec_ntt(&shat,s,PDATA10753);
  for(i=0;i<KEM_K;i++) {
    for(j=0;j<KEM_K;j++) {
      if(transpose)
        neon_ntt(&ahat.vec[j],&a[j].vec[i],PDATA10753);
      else
        neon_ntt(&ahat.vec[j],&a[i].vec[j],PDATA10753);
    }
    polyvec_basemul_acc_montgomery(&t1.vec[i],&ahat,&shat,PDATA10753);
  }

  polyvec_invntt_tomont(&z0,&t0,PDATA7681);
  polyvec_invntt_tomont(&z1,&t1,PDATA10753);
  polyvec_crt(t,&z0,&z1);
}

void polyvec_iprod(poly *r, const polyvec *a, const polyvec *b) {
  nttpoly r0, r1;
  poly z0, z1;
  nttpolyvec ahat;
  nttpolyvec bhat;

  polyvec_ntt(&ahat,a,PDATA7681);
  polyvec_ntt(&bhat,b,PDATA7681);
  polyvec_basemul_acc_montgomery(&r0,&ahat,&bhat,PDATA7681);

  polyvec_ntt(&ahat,a,PDATA10753);
  polyvec_ntt(&bhat,b,PDATA10753);
  polyvec_basemul_acc_montgomery(&r1,&ahat,&bhat,PDATA10753);

  neon_invntt(&z0,&r0,PDATA7681);
  neon_invntt(&z1,&r1,PDATA10753);
  neon_poly_crt(r,&z0,&z1);
}


/* 
#include <stdio.h>

gcc -o neo n_poly neon_poly.c neon_ntt.c consts256n7681.c consts256n10753.c neon_polyvec.c  -g3 -O0; ./neon_poly | md5
#define PP P10753
#define PPDATA PDATA10753

int main()
{
  nttpolyvec ahat, bhat;
  nttpoly r;

  int16_t aa = 4712;
  int16_t bb = 4914;
  for (int j = 0; j < KEM_K; j++)
  {
    for (int i = 0; i < POLY_N; i++)
    {
      ahat.vec[j].coeffs[i] = aa % PP;
      bhat.vec[j].coeffs[i] = bb % PP;
      aa = (aa * 0x1337) + 13 * i;
      bb = (bb * 0x1415) + 13 * i;
      aa += bb;
    }
  }

  polyvec_basemul_acc_montgomery(&r, &ahat, &bhat, PPDATA);

  printf("output = [");
  for (int i = 0; i < KEM_N; i++)
  {
    printf("%d, ", (int16_t)(r.coeffs[i]));
  }
  printf("]\n");
}
Correct 
 */
