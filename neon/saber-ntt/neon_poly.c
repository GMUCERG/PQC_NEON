#include <arm_neon.h>
#include "params.h"
#include "consts256.h"
#include "poly.h"

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

#define fqmul(out, in, zeta, t, neon_p, neon_pinv)                                                        \
    t.val[0] = (int16x8_t)vmull_s16(vget_low_s16(in), vget_low_s16(zeta));                                \
    t.val[1] = (int16x8_t)vmull_high_s16(in, zeta);                                                       \
    t.val[2] = vuzp1q_s16(t.val[0], t.val[1]);                                     /* a_L  */             \
    t.val[3] = vuzp2q_s16(t.val[0], t.val[1]);                                     /* a_H  */             \
    t.val[0] = vmulq_s16(t.val[2], neon_pinv);                                     /* a_L = a_L * QINV */ \
    t.val[1] = (int16x8_t)vmull_s16(vget_low_s16(t.val[0]), vget_low_s16(neon_p)); /* t_L = a_L * Q */    \
    t.val[2] = (int16x8_t)vmull_high_s16(t.val[0], neon_p);                        /* t_H = a_L * Q*/     \
    t.val[0] = vuzp2q_s16(t.val[1], t.val[2]);                                     /* t_H */              \
    out = vsubq_s16(t.val[3], t.val[0]);                                           /* t_H - a_H */
/*************************************************/

// a: ntt 7681, b: ntt 10753
void neon_poly_crt(poly *r, const poly *a, const poly *b)
{
    int16x8_t neon_crt_u, neon_saberq, neon_p7681, neon_p7681_inv,
        neon_p10753, neon_p10753_inv, neon_mont_p7681;
    int16x8x4_t va, vb, vr, t;

    neon_crt_u = vdupq_n_s16(CRT_U);
    neon_saberq = vdupq_n_s16(KEM_Q - 1);
    neon_p7681 = vld1q_s16(&PDATA7681[_8XP]);
    neon_p7681_inv = vld1q_s16(&PDATA7681[_8XPINV]);
    neon_mont_p7681 = vld1q_s16(&PDATA7681[_8XMONT]);
    neon_p10753 = vld1q_s16(&PDATA10753[_8XP]);
    neon_p10753_inv = vld1q_s16(&PDATA10753[_8XPINV]);

    for (int i = 0; i < KEM_N; i += 32)
    {
        vloadx4(va, &a->coeffs[i]);
        vloadx4(vb, &b->coeffs[i]);

        fqmul(va.val[0], va.val[0], neon_mont_p7681, t, neon_p7681, neon_p7681_inv);
        fqmul(va.val[1], va.val[1], neon_mont_p7681, t, neon_p7681, neon_p7681_inv);
        fqmul(va.val[2], va.val[2], neon_mont_p7681, t, neon_p7681, neon_p7681_inv);
        fqmul(va.val[3], va.val[3], neon_mont_p7681, t, neon_p7681, neon_p7681_inv);

        vsub8(vb.val[0], vb.val[0], va.val[0]);
        vsub8(vb.val[1], vb.val[1], va.val[1]);
        vsub8(vb.val[2], vb.val[2], va.val[2]);
        vsub8(vb.val[3], vb.val[3], va.val[3]);

        fqmul(vb.val[0], vb.val[0], neon_crt_u, t, neon_p10753, neon_p10753_inv);
        fqmul(vb.val[1], vb.val[1], neon_crt_u, t, neon_p10753, neon_p10753_inv);
        fqmul(vb.val[2], vb.val[2], neon_crt_u, t, neon_p10753, neon_p10753_inv);
        fqmul(vb.val[3], vb.val[3], neon_crt_u, t, neon_p10753, neon_p10753_inv);

        vb.val[0] = vmulq_s16(vb.val[0], neon_p7681);
        vb.val[1] = vmulq_s16(vb.val[1], neon_p7681);
        vb.val[2] = vmulq_s16(vb.val[2], neon_p7681);
        vb.val[3] = vmulq_s16(vb.val[3], neon_p7681);

        vadd8(vr.val[0], vb.val[0], va.val[0]);
        vadd8(vr.val[1], vb.val[1], va.val[1]);
        vadd8(vr.val[2], vb.val[2], va.val[2]);
        vadd8(vr.val[3], vb.val[3], va.val[3]);

        vand8(vr.val[0], vr.val[0], neon_saberq);
        vand8(vr.val[1], vr.val[1], neon_saberq);
        vand8(vr.val[2], vr.val[2], neon_saberq);
        vand8(vr.val[3], vr.val[3], neon_saberq);

        vstorex4(&r->coeffs[i], vr);
    }
}

/* 
#include <stdio.h>
#include <stdlib.h>

gcc -o neon_poly consts256n7681.c consts256n10753.c neon_poly.c -DSABER=1 -g3 -O0; ./neon_poly | md5
int main()
{
    int i;
    poly a;
    poly crt_a, crt_b;
    srand(0);

    int16_t aa = 4712;
    int16_t bb = 4914;
    for (i = 0; i < POLY_N; i++)
    {
        crt_a.coeffs[i] = aa % P7681;
        crt_b.coeffs[i] = bb % P10753;
        aa = (aa * 0x1337) + 13 * i;
        bb = (bb * 0x1415) + 13 * i;
        aa += bb;
    }

    printf("crt_a = [");
    for (i = 0; i < 20; i++)
    {
        printf("%d, ", (int16_t)(crt_a.coeffs[i]));
    }
    printf("]\n");

    printf("crt_b = [");
    for (i = 0; i < 20; i++)
    {
        printf("%d, ", (int16_t)(crt_b.coeffs[i]));
    }
    printf("]\n");

    neon_poly_crt(&a, &crt_a, &crt_b);

    printf("output = [");
    for (i = 0; i < KEM_N; i++)
    {
        printf("%d, ", (int16_t)(a.coeffs[i]));
    }
    printf("]\n");
}
 */
