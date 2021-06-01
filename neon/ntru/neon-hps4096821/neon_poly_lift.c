#include <arm_neon.h>
#include "poly.h"

// load c <= a
#define poly_vload(c, a) c = vld1q_u16_x4(a);

// c = a | b
#define poly_vor(c, a, b) c = vorrq_u16(a, b);

// c = a >> n
#define poly_vsr(c, a, n) c = vshrq_n_u16(a, n);

// c = a & b
#define poly_vand(c, a, b) c = vandq_u16(a, b);

// c = -a
#define poly_vneg(c, a) c = (uint16x8_t)vnegq_s16((int16x8_t)a);

// c = a ^ b
#define poly_vxor(c, a, b) c = veorq_u16(a, b);

// store [ptr] <= c
#define poly_vstore(ptr, c) vst1q_u16_x4(ptr, c);

// c = a + b
#define poly_vadd(c, a, b) c = vaddq_u16(a, b);

// c = a - b
#define poly_vsub(c, a, b) c = vsubq_u16(a, b);

void poly_lift(poly *r, const poly *a)
{
    uint16x8x4_t neon_r, tmp;
    uint16x8_t const_q;
    const_q = vdupq_n_u16(NTRU_Q - 1);

    for (int i = 0; i < NTRU_N_32; i += 32)
    {
        poly_vload(neon_r, &a->coeffs[i]);

        poly_vsr(tmp.val[0], neon_r.val[0], 1);
        poly_vsr(tmp.val[1], neon_r.val[1], 1);
        poly_vsr(tmp.val[2], neon_r.val[2], 1);
        poly_vsr(tmp.val[3], neon_r.val[3], 1);

        poly_vneg(tmp.val[0], tmp.val[0]);
        poly_vneg(tmp.val[1], tmp.val[1]);
        poly_vneg(tmp.val[2], tmp.val[2]);
        poly_vneg(tmp.val[3], tmp.val[3]);

        poly_vand(tmp.val[0], tmp.val[0], const_q);
        poly_vand(tmp.val[1], tmp.val[1], const_q);
        poly_vand(tmp.val[2], tmp.val[2], const_q);
        poly_vand(tmp.val[3], tmp.val[3], const_q);

        poly_vor(tmp.val[0], tmp.val[0], neon_r.val[0]);
        poly_vor(tmp.val[1], tmp.val[1], neon_r.val[1]);
        poly_vor(tmp.val[2], tmp.val[2], neon_r.val[2]);
        poly_vor(tmp.val[3], tmp.val[3], neon_r.val[3]);

        poly_vstore(&r->coeffs[i], tmp);
    }
}

void poly_lift_add(poly *ct, const poly *a)
{
    uint16x8x4_t neon_r, tmp, neon_ct;
    uint16x8_t const_q;
    const_q = vdupq_n_u16(NTRU_Q - 1);

    for (int i = 0; i < NTRU_N_32; i += 32)
    {
        poly_vload(neon_r, &a->coeffs[i]);
        poly_vload(neon_ct, &ct->coeffs[i]);

        poly_vsr(tmp.val[0], neon_r.val[0], 1);
        poly_vsr(tmp.val[1], neon_r.val[1], 1);
        poly_vsr(tmp.val[2], neon_r.val[2], 1);
        poly_vsr(tmp.val[3], neon_r.val[3], 1);

        poly_vneg(tmp.val[0], tmp.val[0]);
        poly_vneg(tmp.val[1], tmp.val[1]);
        poly_vneg(tmp.val[2], tmp.val[2]);
        poly_vneg(tmp.val[3], tmp.val[3]);

        poly_vand(tmp.val[0], tmp.val[0], const_q);
        poly_vand(tmp.val[1], tmp.val[1], const_q);
        poly_vand(tmp.val[2], tmp.val[2], const_q);
        poly_vand(tmp.val[3], tmp.val[3], const_q);

        poly_vor(tmp.val[0], tmp.val[0], neon_r.val[0]);
        poly_vor(tmp.val[1], tmp.val[1], neon_r.val[1]);
        poly_vor(tmp.val[2], tmp.val[2], neon_r.val[2]);
        poly_vor(tmp.val[3], tmp.val[3], neon_r.val[3]);

        poly_vadd(neon_ct.val[0], neon_ct.val[0], tmp.val[0]);
        poly_vadd(neon_ct.val[1], neon_ct.val[1], tmp.val[1]);
        poly_vadd(neon_ct.val[2], neon_ct.val[2], tmp.val[2]);
        poly_vadd(neon_ct.val[3], neon_ct.val[3], tmp.val[3]);

        poly_vstore(&ct->coeffs[i], neon_ct);
    }
}

void poly_lift_sub(poly *b, const poly *c, const poly *a)
{
    uint16x8x4_t neon_r, tmp, neon_ct;
    uint16x8_t const_q;
    const_q = vdupq_n_u16(NTRU_Q - 1);

    for (int i = 0; i < NTRU_N_32; i += 32)
    {
        poly_vload(neon_r, &a->coeffs[i]);
        poly_vload(neon_ct, &c->coeffs[i]);

        poly_vsr(tmp.val[0], neon_r.val[0], 1);
        poly_vsr(tmp.val[1], neon_r.val[1], 1);
        poly_vsr(tmp.val[2], neon_r.val[2], 1);
        poly_vsr(tmp.val[3], neon_r.val[3], 1);

        poly_vneg(tmp.val[0], tmp.val[0]);
        poly_vneg(tmp.val[1], tmp.val[1]);
        poly_vneg(tmp.val[2], tmp.val[2]);
        poly_vneg(tmp.val[3], tmp.val[3]);

        poly_vand(tmp.val[0], tmp.val[0], const_q);
        poly_vand(tmp.val[1], tmp.val[1], const_q);
        poly_vand(tmp.val[2], tmp.val[2], const_q);
        poly_vand(tmp.val[3], tmp.val[3], const_q);

        poly_vor(tmp.val[0], tmp.val[0], neon_r.val[0]);
        poly_vor(tmp.val[1], tmp.val[1], neon_r.val[1]);
        poly_vor(tmp.val[2], tmp.val[2], neon_r.val[2]);
        poly_vor(tmp.val[3], tmp.val[3], neon_r.val[3]);

        poly_vsub(neon_ct.val[0], neon_ct.val[0], tmp.val[0]);
        poly_vsub(neon_ct.val[1], neon_ct.val[1], tmp.val[1]);
        poly_vsub(neon_ct.val[2], neon_ct.val[2], tmp.val[2]);
        poly_vsub(neon_ct.val[3], neon_ct.val[3], tmp.val[3]);

        poly_vstore(&b->coeffs[i], neon_ct);
    }
}

void poly_Z3_to_Zq(poly *r)
{
    uint16x8x4_t neon_r, tmp1, tmp2;
    uint16x8_t const_q;
    const_q = vdupq_n_u16(NTRU_Q - 1);

    for (int i = 0; i < NTRU_N_32; i += 32)
    {
        poly_vload(neon_r, &r->coeffs[i]);

        poly_vsr(tmp1.val[0], neon_r.val[0], 1);
        poly_vsr(tmp1.val[1], neon_r.val[1], 1);
        poly_vsr(tmp1.val[2], neon_r.val[2], 1);
        poly_vsr(tmp1.val[3], neon_r.val[3], 1);

        poly_vneg(tmp1.val[0], tmp1.val[0]);
        poly_vneg(tmp1.val[1], tmp1.val[1]);
        poly_vneg(tmp1.val[2], tmp1.val[2]);
        poly_vneg(tmp1.val[3], tmp1.val[3]);

        poly_vand(tmp2.val[0], tmp1.val[0], const_q);
        poly_vand(tmp2.val[1], tmp1.val[1], const_q);
        poly_vand(tmp2.val[2], tmp1.val[2], const_q);
        poly_vand(tmp2.val[3], tmp1.val[3], const_q);

        poly_vor(tmp2.val[0], tmp2.val[0], neon_r.val[0]);
        poly_vor(tmp2.val[1], tmp2.val[1], neon_r.val[1]);
        poly_vor(tmp2.val[2], tmp2.val[2], neon_r.val[2]);
        poly_vor(tmp2.val[3], tmp2.val[3], neon_r.val[3]);

        poly_vstore(&r->coeffs[i], tmp2);
    }
}

void poly_trinary_Zq_to_Z3(poly *r)
{
    uint16x8x4_t neon_r, tmp1, tmp2;
    uint16x8_t const_q, const_3;

    const_q = vdupq_n_u16(NTRU_Q - 1);
    const_3 = vdupq_n_u16(3);
    for (int i = 0; i < NTRU_N_32; i += 32)
    {
        poly_vload(neon_r, &r->coeffs[i]);

        poly_vand(neon_r.val[0], neon_r.val[0], const_q);
        poly_vand(neon_r.val[1], neon_r.val[1], const_q);
        poly_vand(neon_r.val[2], neon_r.val[2], const_q);
        poly_vand(neon_r.val[3], neon_r.val[3], const_q);

        poly_vsr(tmp1.val[0], neon_r.val[0], NTRU_LOGQ - 1);
        poly_vsr(tmp1.val[1], neon_r.val[1], NTRU_LOGQ - 1);
        poly_vsr(tmp1.val[2], neon_r.val[2], NTRU_LOGQ - 1);
        poly_vsr(tmp1.val[3], neon_r.val[3], NTRU_LOGQ - 1);

        poly_vxor(tmp1.val[0], neon_r.val[0], tmp1.val[0]);
        poly_vxor(tmp1.val[1], neon_r.val[1], tmp1.val[1]);
        poly_vxor(tmp1.val[2], neon_r.val[2], tmp1.val[2]);
        poly_vxor(tmp1.val[3], neon_r.val[3], tmp1.val[3]);

        poly_vand(tmp2.val[0], tmp1.val[0], const_3);
        poly_vand(tmp2.val[1], tmp1.val[1], const_3);
        poly_vand(tmp2.val[2], tmp1.val[2], const_3);
        poly_vand(tmp2.val[3], tmp1.val[3], const_3);

        poly_vstore(&r->coeffs[i], tmp2);
    }
}

void polyhps_mul3(poly *g)
{
    uint16x8x4_t neon_g;
    for (int i = 0; i < NTRU_N_32; i += 32)
    {
        neon_g = vld1q_u16_x4(&g->coeffs[i]);

        neon_g.val[0] = vmulq_n_u16(neon_g.val[0], 3);
        neon_g.val[1] = vmulq_n_u16(neon_g.val[1], 3);
        neon_g.val[2] = vmulq_n_u16(neon_g.val[2], 3);
        neon_g.val[3] = vmulq_n_u16(neon_g.val[3], 3);

        vst1q_u16_x4(&g->coeffs[i], neon_g);
    }
}

void polyhrss_mul3(poly *g)
{
    uint16x8x4_t neon_g, neon_g_minus;
    uint16_t last = g->coeffs[0];
    for (int i = NTRU_N_32 - 32; i > 32; i -= 32)
    {
        neon_g = vld1q_u16_x4(&g->coeffs[i]);
        neon_g_minus = vld1q_u16_x4(&g->coeffs[i - 1]);

        poly_vsub(neon_g.val[0], neon_g_minus.val[0], neon_g.val[0]);
        poly_vsub(neon_g.val[1], neon_g_minus.val[1], neon_g.val[1]);
        poly_vsub(neon_g.val[2], neon_g_minus.val[2], neon_g.val[2]);
        poly_vsub(neon_g.val[3], neon_g_minus.val[3], neon_g.val[3]);

        neon_g.val[0] = vmulq_n_u16(neon_g.val[0], 3);
        neon_g.val[1] = vmulq_n_u16(neon_g.val[1], 3);
        neon_g.val[2] = vmulq_n_u16(neon_g.val[2], 3);
        neon_g.val[3] = vmulq_n_u16(neon_g.val[3], 3);

        vst1q_u16_x4(&g->coeffs[i], neon_g);
    }
    g->coeffs[0] = -(3 * last);
}