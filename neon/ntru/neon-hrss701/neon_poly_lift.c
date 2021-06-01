#include <arm_neon.h>
#include "poly.h"

// load c <= a
#define poly_vload(c, a) c = vld1q_u16_x4(a);

// c = a | b
#define poly_vor(c, a, b) c = vorrq_u16(a, b);

// c = a >> n
#define poly_vsr(c, a, n) c = vshrq_n_u16(a, n);

// c = a >> n
#define poly_vsl(c, a, n) c = vshlq_n_u16(a, n);

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
    int i;
    poly b;
    uint16x8x3_t neon_a, tmp, sum, neon_3, neon_4, neon_5;

    b.coeffs[0] = a->coeffs[0];
    b.coeffs[0] += a->coeffs[2];

    b.coeffs[1] = a->coeffs[1];
    b.coeffs[1] += a->coeffs[0] * 3;

    b.coeffs[2] = a->coeffs[2];
    b.coeffs[2] += a->coeffs[0] * 2;
    b.coeffs[2] += a->coeffs[1] * 3;

    for (i = 3; i < NTRU_N - 24; i += 24)
    {
        // 3, 6, 9 ...
        // 4, 7, 10...
        // 5, 8, 11...
        neon_a = vld3q_u16(&a->coeffs[i]);

        /////// [4 3 2]
        poly_vsl(tmp.val[0], neon_a.val[0], 1);

        poly_vsl(tmp.val[1], neon_a.val[1], 1);
        poly_vadd(tmp.val[1], tmp.val[1], neon_a.val[1]);

        poly_vsl(tmp.val[2], neon_a.val[2], 2);

        poly_vadd(sum.val[0], tmp.val[0], tmp.val[1]);
        poly_vadd(sum.val[0], sum.val[0], tmp.val[2]);

        /////// [3 2 1]
        poly_vsl(tmp.val[1], neon_a.val[1], 1);

        poly_vsl(tmp.val[2], neon_a.val[2], 1);
        poly_vadd(tmp.val[2], tmp.val[2], neon_a.val[2]);

        poly_vadd(sum.val[1], tmp.val[1], neon_a.val[0]);
        poly_vadd(sum.val[1], sum.val[1], tmp.val[2]);

        /////// [2 1 0]
        poly_vsl(tmp.val[2], neon_a.val[2], 1);
        poly_vadd(sum.val[2], neon_a.val[1], tmp.val[2]);

        b.coeffs[0] += vaddvq_u16(sum.val[0]);
        b.coeffs[1] += vaddvq_u16(sum.val[1]);
        b.coeffs[2] += vaddvq_u16(sum.val[2]);
    }

    b.coeffs[0] += a->coeffs[699] * 2;
    b.coeffs[1] += a->coeffs[699] * 1;
    b.coeffs[2] += a->coeffs[699] * 0;

    b.coeffs[0] += a->coeffs[700] * 3;
    b.coeffs[1] += a->coeffs[700] * 2;
    b.coeffs[2] += a->coeffs[700] * 1;

    for (i = 3; i < NTRU_N - 24; i += 24)
    {
        neon_3 = vld3q_u16(&a->coeffs[i - 2]);
        neon_4 = vld3q_u16(&a->coeffs[i - 1]);
        neon_5 = vld3q_u16(&a->coeffs[i]);

        // 3k
        poly_vadd(sum.val[0], neon_3.val[0], neon_3.val[1]);
        poly_vadd(sum.val[0], sum.val[0], neon_3.val[2]);
        poly_vsl(sum.val[0], sum.val[0], 1);

        // 3k
        b.coeffs[i + 3 * 0] = b.coeffs[i - 3 * 1] + vgetq_lane_u16(sum.val[0], 0);
        b.coeffs[i + 3 * 1] = b.coeffs[i + 3 * 0] + vgetq_lane_u16(sum.val[0], 1);
        b.coeffs[i + 3 * 2] = b.coeffs[i + 3 * 1] + vgetq_lane_u16(sum.val[0], 2);
        b.coeffs[i + 3 * 3] = b.coeffs[i + 3 * 2] + vgetq_lane_u16(sum.val[0], 3);
        b.coeffs[i + 3 * 4] = b.coeffs[i + 3 * 3] + vgetq_lane_u16(sum.val[0], 4);
        b.coeffs[i + 3 * 5] = b.coeffs[i + 3 * 4] + vgetq_lane_u16(sum.val[0], 5);
        b.coeffs[i + 3 * 6] = b.coeffs[i + 3 * 5] + vgetq_lane_u16(sum.val[0], 6);
        b.coeffs[i + 3 * 7] = b.coeffs[i + 3 * 6] + vgetq_lane_u16(sum.val[0], 7);

        // 3k+1
        poly_vadd(sum.val[1], neon_4.val[0], neon_4.val[1]);
        poly_vadd(sum.val[1], sum.val[1], neon_4.val[2]);
        poly_vsl(sum.val[1], sum.val[1], 1);

        b.coeffs[1 + i + 3 * 0] = b.coeffs[1 + i - 3 * 1] + vgetq_lane_u16(sum.val[1], 0);
        b.coeffs[1 + i + 3 * 1] = b.coeffs[1 + i + 3 * 0] + vgetq_lane_u16(sum.val[1], 1);
        b.coeffs[1 + i + 3 * 2] = b.coeffs[1 + i + 3 * 1] + vgetq_lane_u16(sum.val[1], 2);
        b.coeffs[1 + i + 3 * 3] = b.coeffs[1 + i + 3 * 2] + vgetq_lane_u16(sum.val[1], 3);
        b.coeffs[1 + i + 3 * 4] = b.coeffs[1 + i + 3 * 3] + vgetq_lane_u16(sum.val[1], 4);
        b.coeffs[1 + i + 3 * 5] = b.coeffs[1 + i + 3 * 4] + vgetq_lane_u16(sum.val[1], 5);
        b.coeffs[1 + i + 3 * 6] = b.coeffs[1 + i + 3 * 5] + vgetq_lane_u16(sum.val[1], 6);
        b.coeffs[1 + i + 3 * 7] = b.coeffs[1 + i + 3 * 6] + vgetq_lane_u16(sum.val[1], 7);

        // 3k + 2
        poly_vadd(sum.val[2], neon_5.val[0], neon_5.val[1]);
        poly_vadd(sum.val[2], sum.val[2], neon_5.val[2]);
        poly_vsl(sum.val[2], sum.val[2], 1);

        b.coeffs[2 + i + 3 * 0] = b.coeffs[2 + i - 3 * 1] + vgetq_lane_u16(sum.val[2], 0);
        b.coeffs[2 + i + 3 * 1] = b.coeffs[2 + i + 3 * 0] + vgetq_lane_u16(sum.val[2], 1);
        b.coeffs[2 + i + 3 * 2] = b.coeffs[2 + i + 3 * 1] + vgetq_lane_u16(sum.val[2], 2);
        b.coeffs[2 + i + 3 * 3] = b.coeffs[2 + i + 3 * 2] + vgetq_lane_u16(sum.val[2], 3);
        b.coeffs[2 + i + 3 * 4] = b.coeffs[2 + i + 3 * 3] + vgetq_lane_u16(sum.val[2], 4);
        b.coeffs[2 + i + 3 * 5] = b.coeffs[2 + i + 3 * 4] + vgetq_lane_u16(sum.val[2], 5);
        b.coeffs[2 + i + 3 * 6] = b.coeffs[2 + i + 3 * 5] + vgetq_lane_u16(sum.val[2], 6);
        b.coeffs[2 + i + 3 * 7] = b.coeffs[2 + i + 3 * 6] + vgetq_lane_u16(sum.val[2], 7);
    }
    b.coeffs[699] = b.coeffs[696] + 2*(a->coeffs[697] + a->coeffs[698] + a->coeffs[699]);
    b.coeffs[700] = b.coeffs[697] + 2*(a->coeffs[698] + a->coeffs[699] + a->coeffs[700]);


    /* Finish reduction mod Phi by subtracting Phi * b[N-1] */
    poly_mod_3_Phi_n(&b);

    /* Switch from {0,1,2} to {0,1,q-1} coefficient representation */
    poly_Z3_to_Zq(&b);

    // /* Multiply by (x-1) */
    r->coeffs[0] = -(b.coeffs[0]);
    for (i = 0; i < NTRU_N - 1; i++)
    {
        r->coeffs[i + 1] = b.coeffs[i] - b.coeffs[i + 1];
    }

    // TODO: Debug this code. Out of bound write. 
    // uint16x8x2_t neon_b, neon_b1, res;
    // for (i = 0; i < NTRU_N_32 - 16; i += 16)
    // {
    //     // r->coeffs[i + 1] = b.coeffs[i]   - b.coeffs[i + 1];
    //     // r->coeffs[i + 2] = b.coeffs[i+1] - b.coeffs[i + 2];
    //     neon_b  = vld2q_u16(&b.coeffs[i]);
    //     neon_b1 = vld2q_u16(&b.coeffs[i+1]);

    //     // Odd
    //     poly_vsub(sum.val[0], neon_b.val[0], neon_b.val[1]);
    //     // Even
    //     poly_vsub(sum.val[1], neon_b1.val[0], neon_b1.val[1]);

    //     res.val[0] = vzip1q_u16(sum.val[0], sum.val[1]);
    //     res.val[1] = vzip2q_u16(sum.val[0], sum.val[1]);

    //     vst1q_u16_x2(&r->coeffs[i+1], res);
    //     // printf("%d: %d -- %d\n", i, r->coeffs[i + 1], r->coeffs[i + 2]);
    //     // printf("-----------\n");
    // }

    // printf("\n[");
    // for (i = 0; i < NTRU_N_32; i++)
    // {
    //     printf("%u, ", r->coeffs[i]);
    // }
    // printf("]\n");
}

// // ct += a
// void poly_lift_add(poly *ct, const poly *a)
// {
//     int i;
//     poly b;
//     uint16x8x3_t neon_a, tmp, sum;

//     b.coeffs[0] = a->coeffs[0];
//     b.coeffs[0]+= a->coeffs[2];

//     b.coeffs[1] = a->coeffs[1];
//     b.coeffs[1] += a->coeffs[0] * 3;

//     b.coeffs[2] = a->coeffs[2];
//     b.coeffs[2] += a->coeffs[0] * 2;
//     b.coeffs[2] += a->coeffs[1] * 3;

//     for (i = 3; i < NTRU_N - 1; i+= 24)
//     {
//         // 3, 6, 9 ...
//         // 4, 7, 10...
//         // 5, 8, 11...
//         neon_a = vld3q_u16(&a->coeffs[i]);
//         // [4 3 2]

//         // 3, 6, 9... x2
//         poly_vsl(tmp.val[0], neon_a.val[0], 1);
//         // 4, 7, 10... x3
//         poly_vsl(tmp.val[1], neon_a.val[1], 1);
//         poly_vadd(tmp.val[1], tmp.val[1], neon_a.val[1]);
//         // 5 8 11... x4
//         poly_vsl(tmp.val[2], neon_a.val[2], 2);
//         // 0
//         poly_vadd(sum.val[0], tmp.val[0], tmp.val[1]);
//         poly_vadd(sum.val[0], sum.val[0], tmp.val[2]);

//         /////// [3 2 1]
//         poly_vsl(tmp.val[1], neon_a.val[1], 1);

//         poly_vsl(tmp.val[2], neon_a.val[2], 1);
//         poly_vadd(tmp.val[2], tmp.val[2], neon_a.val[2]);

//         poly_vadd(sum.val[1], tmp.val[1], neon_a.val[0]);
//         poly_vadd(sum.val[1], sum.val[1], tmp.val[2]);

//         ///////// [2 1 0]
//         poly_vsl(tmp.val[2], neon_a.val[2], 1);
//         poly_vadd(sum.val[2], neon_a.val[1], tmp.val[2]);

//         b.coeffs[0] += vaddvq_u16(sum.val[0]);
//         b.coeffs[1] += vaddvq_u16(sum.val[1]);
//         b.coeffs[2] += vaddvq_u16(sum.val[2]);
//     }

//     b.coeffs[0] += a->coeffs[700] * 3;
//     b.coeffs[1] += a->coeffs[700] * 2;
//     b.coeffs[2] += a->coeffs[700] * 1;

//     for (i = 3; i < NTRU_N; i++)
//     {
//         b.coeffs[i] = b.coeffs[i - 3] + 2 * (a->coeffs[i] + a->coeffs[i - 1] + a->coeffs[i - 2]);
//     }

//     /* Finish reduction mod Phi by subtracting Phi * b[N-1] */
//     poly_mod_3_Phi_n(&b);

//     /* Switch from {0,1,2} to {0,1,q-1} coefficient representation */
//     poly_Z3_to_Zq(&b);

//     /* Multiply by (x-1) */
//     ct->coeffs[0] += -(b.coeffs[0]);
//     for (i = 0; i < NTRU_N - 1; i++)
//     {
//         ct->coeffs[i + 1] += b.coeffs[i] - b.coeffs[i + 1];
//     }
// }

// // b = c - a
// void poly_lift_sub(poly *bb, const poly *c, const poly *a)
// {
//     int i;
//     poly b;
//     uint16x8x3_t neon_a, tmp, sum;

//     b.coeffs[0] = a->coeffs[0];
//     b.coeffs[0]+= a->coeffs[2];

//     b.coeffs[1] = a->coeffs[1];
//     b.coeffs[1] += a->coeffs[0] * 3;

//     b.coeffs[2] = a->coeffs[2];
//     b.coeffs[2] += a->coeffs[0] * 2;
//     b.coeffs[2] += a->coeffs[1] * 3;

//     for (i = 3; i < NTRU_N - 1; i+= 24)
//     {
//         // 3, 6, 9 ...
//         // 4, 7, 10...
//         // 5, 8, 11...
//         neon_a = vld3q_u16(&a->coeffs[i]);
//         // [4 3 2]

//         // 3, 6, 9... x2
//         poly_vsl(tmp.val[0], neon_a.val[0], 1);
//         // 4, 7, 10... x3
//         poly_vsl(tmp.val[1], neon_a.val[1], 1);
//         poly_vadd(tmp.val[1], tmp.val[1], neon_a.val[1]);
//         // 5 8 11... x4
//         poly_vsl(tmp.val[2], neon_a.val[2], 2);
//         // 0
//         poly_vadd(sum.val[0], tmp.val[0], tmp.val[1]);
//         poly_vadd(sum.val[0], sum.val[0], tmp.val[2]);

//         /////// [3 2 1]
//         poly_vsl(tmp.val[1], neon_a.val[1], 1);

//         poly_vsl(tmp.val[2], neon_a.val[2], 1);
//         poly_vadd(tmp.val[2], tmp.val[2], neon_a.val[2]);

//         poly_vadd(sum.val[1], tmp.val[1], neon_a.val[0]);
//         poly_vadd(sum.val[1], sum.val[1], tmp.val[2]);

//         ///////// [2 1 0]
//         poly_vsl(tmp.val[2], neon_a.val[2], 1);
//         poly_vadd(sum.val[2], neon_a.val[1], tmp.val[2]);

//         b.coeffs[0] += vaddvq_u16(sum.val[0]);
//         b.coeffs[1] += vaddvq_u16(sum.val[1]);
//         b.coeffs[2] += vaddvq_u16(sum.val[2]);
//     }

//     b.coeffs[0] += a->coeffs[700] * 3;
//     b.coeffs[1] += a->coeffs[700] * 2;
//     b.coeffs[2] += a->coeffs[700] * 1;

//     for (i = 3; i < NTRU_N; i++)
//     {
//         b.coeffs[i] = b.coeffs[i - 3] + 2 * (a->coeffs[i] + a->coeffs[i - 1] + a->coeffs[i - 2]);
//     }

//     /* Finish reduction mod Phi by subtracting Phi * b[N-1] */
//     poly_mod_3_Phi_n(&b);

//     /* Switch from {0,1,2} to {0,1,q-1} coefficient representation */
//     poly_Z3_to_Zq(&b);

//     /* Multiply by (x-1) */
//     bb->coeffs[0] -= -(b.coeffs[0]);
//     for (i = 0; i < NTRU_N - 1; i++)
//     {
//         bb->coeffs[i + 1] -= b.coeffs[i] - b.coeffs[i + 1];
//     }
//     for (i  = 0; i < NTRU_N; i++)
//     {
//         bb->coeffs[i] -= c->coeffs[i] - r[i];
//     }
// }

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
