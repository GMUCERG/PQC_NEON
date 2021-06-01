#include <arm_neon.h>
#include "consts256.h"
#include "neon_ntt.h"

/*************************************************/
// Load int16x8_t c <= ptr*
#define vload(c, ptr) c = vld1q_s16(ptr);

// Load int16x8_t c <= ptr*
#define vloadx4(c, ptr) c = vld1q_s16_x4(ptr);

// Store *ptr <= c
#define vstorex4(ptr, c) vst1q_s16_x4(ptr, c);

// Load int16x8_t c <= ptr*
#define vload4(c, ptr) c = vld4q_s16(ptr);

// Store *ptr <= c
#define vstore4(ptr, c) vst4q_s16(ptr, c);

// c (int16x8) = a + b (int16x8)
#define vadd8(c, a, b) c = vaddq_s16(a, b);

// c (int16x8) = a - b (int16x8)
#define vsub8(c, a, b) c = vsubq_s16(a, b);

// c = a
#define vcopy(c, a) c = vorrq_s16(a, a);
/*************************************************/

/*
Re-arrange vector
*/
#define arrange(v_out1, v_out2, v1, v2)                         \
  v_out1 = (int16x8_t)vtrn1q_s64((int64x2_t)v1, (int64x2_t)v2); \
  v_out2 = (int16x8_t)vtrn2q_s64((int64x2_t)v1, (int64x2_t)v2);
/*************************************************
* Name:        fqmul
*
* Description: Multiplication followed by Montgomery reduction
*
* Arguments:   - int16_t a: first factor
*              - int16_t b: second factor
*
* Returns 16-bit integer congruent to a*b*R^{-1} mod q

out, in: int16x8_t
zeta: input : int16x8_t
t : int16x8x4_t
neon_qinv: const   : int16x8_t
neon_kyberq: const : int16x8_t
rewrite pseudo code:
int16_t fqmul(int16_t b, int16_t c) {
  int32_t t, u, a;

  a = (int32_t) b*c;
  (a_L, a_H) = a
  a_L = a_L * QINV;
  t = a_L * Q;
  (t_L, t_H) = t;
  return t_H - a_H;
}
*************************************************/
#define fqmul(out, in, zeta, t)                                                                         \
  t.val[0] = (int16x8_t)vmull_s16(vget_low_s16(in), vget_low_s16(zeta));                                \
  t.val[1] = (int16x8_t)vmull_high_s16(in, zeta);                                                       \
  t.val[2] = vuzp1q_s16(t.val[0], t.val[1]);                                     /* a_L  */             \
  t.val[3] = vuzp2q_s16(t.val[0], t.val[1]);                                     /* a_H  */             \
  t.val[0] = vmulq_s16(t.val[2], neon_pinv);                                     /* a_L = a_L * QINV */ \
  t.val[1] = (int16x8_t)vmull_s16(vget_low_s16(t.val[0]), vget_low_s16(neon_p)); /* t_L = a_L * Q */    \
  t.val[2] = (int16x8_t)vmull_high_s16(t.val[0], neon_p);                        /* t_H = a_L * Q*/     \
  t.val[0] = vuzp2q_s16(t.val[1], t.val[2]);                                     /* t_H */              \
  out = vsubq_s16(t.val[3], t.val[0]);                                           /* t_H - a_H */

#define subadd_x4(v2, v0, t)             \
  vcopy(t.val[0], v2.val[0]);            \
  vcopy(t.val[1], v2.val[1]);            \
  vcopy(t.val[2], v2.val[2]);            \
  vcopy(t.val[3], v2.val[3]);            \
  vsub8(v2.val[0], v0.val[0], t.val[0]); \
  vsub8(v2.val[1], v0.val[1], t.val[1]); \
  vsub8(v2.val[2], v0.val[2], t.val[2]); \
  vsub8(v2.val[3], v0.val[3], t.val[3]); \
  vadd8(v0.val[0], v0.val[0], t.val[0]); \
  vadd8(v0.val[1], v0.val[1], t.val[1]); \
  vadd8(v0.val[2], v0.val[2], t.val[2]); \
  vadd8(v0.val[3], v0.val[3], t.val[3]);

#define subadd(v, i, j, m, n, t, k)        \
  vcopy(t.val[k], v.val[i]);               \
  vcopy(t.val[k + 1], v.val[m]);           \
  vadd8(v.val[i], t.val[k], v.val[j]);     \
  vadd8(v.val[m], t.val[k + 1], v.val[n]); \
  vsub8(v.val[j], t.val[k], v.val[j]);     \
  vsub8(v.val[n], t.val[k + 1], v.val[n]);

#define transpose(v0, v1, v2, v3, tmp)                                      \
  tmp.val[0] = vtrn1q_s16(v0, v1);                                          \
  tmp.val[1] = vtrn2q_s16(v0, v1);                                          \
  tmp.val[2] = vtrn1q_s16(v2, v3);                                          \
  tmp.val[3] = vtrn2q_s16(v2, v3);                                          \
  v0 = (int16x8_t)vtrn1q_s32((int32x4_t)tmp.val[0], (int32x4_t)tmp.val[2]); \
  v2 = (int16x8_t)vtrn2q_s32((int32x4_t)tmp.val[0], (int32x4_t)tmp.val[2]); \
  v1 = (int16x8_t)vtrn1q_s32((int32x4_t)tmp.val[1], (int32x4_t)tmp.val[3]); \
  v3 = (int16x8_t)vtrn2q_s32((int32x4_t)tmp.val[1], (int32x4_t)tmp.val[3]);

void neon_ntt(nttpoly *out, const poly *in, const int16_t *pdata)
{
  int i;
  int16x8x4_t v0, v1, v2, v3, t, z;
  int16x8x4_t vt0, vt1;
  int16x8_t neon_p, neon_pinv;

  neon_p = vld1q_s16(&pdata[_8XP]);
  neon_pinv = vld1q_s16(&pdata[_8XPINV]);

  z.val[0] = vdupq_n_s16(pdata[_ZETAS + 0]);
  // level 0
  for (i = 0; i < 128; i += 64)
  {
    vloadx4(v0, &in->coeffs[i]);
    vloadx4(v1, &in->coeffs[i + 32]);
    vloadx4(v2, &in->coeffs[i + 128]);
    vloadx4(v3, &in->coeffs[i + 160]);

    // v2
    fqmul(vt0.val[0], v2.val[0], z.val[0], t);
    fqmul(vt0.val[1], v2.val[1], z.val[0], t);
    fqmul(vt0.val[2], v2.val[2], z.val[0], t);
    fqmul(vt0.val[3], v2.val[3], z.val[0], t);

    // v3
    fqmul(vt1.val[0], v3.val[0], z.val[0], t);
    fqmul(vt1.val[1], v3.val[1], z.val[0], t);
    fqmul(vt1.val[2], v3.val[2], z.val[0], t);
    fqmul(vt1.val[3], v3.val[3], z.val[0], t);

    // v2 = v0 - v2
    vsub8(v2.val[0], v0.val[0], vt0.val[0]);
    vsub8(v2.val[1], v0.val[1], vt0.val[1]);
    vsub8(v2.val[2], v0.val[2], vt0.val[2]);
    vsub8(v2.val[3], v0.val[3], vt0.val[3]);

    // v0 = v2 + v0
    vadd8(v0.val[0], v0.val[0], vt0.val[0]);
    vadd8(v0.val[1], v0.val[1], vt0.val[1]);
    vadd8(v0.val[2], v0.val[2], vt0.val[2]);
    vadd8(v0.val[3], v0.val[3], vt0.val[3]);

    // v3 = v3 - v1
    vsub8(v3.val[0], v1.val[0], vt1.val[0]);
    vsub8(v3.val[1], v1.val[1], vt1.val[1]);
    vsub8(v3.val[2], v1.val[2], vt1.val[2]);
    vsub8(v3.val[3], v1.val[3], vt1.val[3]);
    // v1 = v3 + v1
    vadd8(v1.val[0], v1.val[0], vt1.val[0]);
    vadd8(v1.val[1], v1.val[1], vt1.val[1]);
    vadd8(v1.val[2], v1.val[2], vt1.val[2]);
    vadd8(v1.val[3], v1.val[3], vt1.val[3]);

    vstorex4(&out->coeffs[i], v0);
    vstorex4(&out->coeffs[i + 32], v1);
    vstorex4(&out->coeffs[i + 128], v2);
    vstorex4(&out->coeffs[i + 160], v3);
  }

  for (i = 0; i < 256; i += 128)
  {
    // level 1
    vloadx4(v0, &out->coeffs[i]);
    vloadx4(v1, &out->coeffs[i + 32]);
    vloadx4(v2, &out->coeffs[i + 64]);
    vloadx4(v3, &out->coeffs[i + 96]);

    z.val[0] = vdupq_n_s16(pdata[_ZETAS + 1 + (i >> 7)]);

    // v2
    fqmul(vt0.val[0], v2.val[0], z.val[0], t);
    fqmul(vt0.val[1], v2.val[1], z.val[0], t);
    fqmul(vt0.val[2], v2.val[2], z.val[0], t);
    fqmul(vt0.val[3], v2.val[3], z.val[0], t);

    // v3
    fqmul(vt1.val[0], v3.val[0], z.val[0], t);
    fqmul(vt1.val[1], v3.val[1], z.val[0], t);
    fqmul(vt1.val[2], v3.val[2], z.val[0], t);
    fqmul(vt1.val[3], v3.val[3], z.val[0], t);

    // v2 = v0 - v2
    vsub8(v2.val[0], v0.val[0], vt0.val[0]);
    vsub8(v2.val[1], v0.val[1], vt0.val[1]);
    vsub8(v2.val[2], v0.val[2], vt0.val[2]);
    vsub8(v2.val[3], v0.val[3], vt0.val[3]);

    // v0 = v2 + v0
    vadd8(v0.val[0], v0.val[0], vt0.val[0]);
    vadd8(v0.val[1], v0.val[1], vt0.val[1]);
    vadd8(v0.val[2], v0.val[2], vt0.val[2]);
    vadd8(v0.val[3], v0.val[3], vt0.val[3]);

    // v3 = v3 - v1
    vsub8(v3.val[0], v1.val[0], vt1.val[0]);
    vsub8(v3.val[1], v1.val[1], vt1.val[1]);
    vsub8(v3.val[2], v1.val[2], vt1.val[2]);
    vsub8(v3.val[3], v1.val[3], vt1.val[3]);
    // v1 = v3 + v1
    vadd8(v1.val[0], v1.val[0], vt1.val[0]);
    vadd8(v1.val[1], v1.val[1], vt1.val[1]);
    vadd8(v1.val[2], v1.val[2], vt1.val[2]);
    vadd8(v1.val[3], v1.val[3], vt1.val[3]);

    // level 2
    z.val[0] = vdupq_n_s16(pdata[_ZETAS + 3 + (i >> 6)]);
    z.val[1] = vdupq_n_s16(pdata[_ZETAS + 4 + (i >> 6)]);

    // v1
    fqmul(vt0.val[0], v1.val[0], z.val[0], t);
    fqmul(vt0.val[1], v1.val[1], z.val[0], t);
    fqmul(vt0.val[2], v1.val[2], z.val[0], t);
    fqmul(vt0.val[3], v1.val[3], z.val[0], t);

    // v3
    fqmul(vt1.val[0], v3.val[0], z.val[1], t);
    fqmul(vt1.val[1], v3.val[1], z.val[1], t);
    fqmul(vt1.val[2], v3.val[2], z.val[1], t);
    fqmul(vt1.val[3], v3.val[3], z.val[1], t);

    // v0 x v1
    // v1 = v0 - v1
    vsub8(v1.val[0], v0.val[0], vt0.val[0]);
    vsub8(v1.val[1], v0.val[1], vt0.val[1]);
    vsub8(v1.val[2], v0.val[2], vt0.val[2]);
    vsub8(v1.val[3], v0.val[3], vt0.val[3]);
    // v0 = v0 + v1
    vadd8(v0.val[0], v0.val[0], vt0.val[0]);
    vadd8(v0.val[1], v0.val[1], vt0.val[1]);
    vadd8(v0.val[2], v0.val[2], vt0.val[2]);
    vadd8(v0.val[3], v0.val[3], vt0.val[3]);

    // v2 x v3
    // v3 = v2 - v3
    vsub8(v3.val[0], v2.val[0], vt1.val[0]);
    vsub8(v3.val[1], v2.val[1], vt1.val[1]);
    vsub8(v3.val[2], v2.val[2], vt1.val[2]);
    vsub8(v3.val[3], v2.val[3], vt1.val[3]);
    // v2 = v3 + v2
    vadd8(v2.val[0], v2.val[0], vt1.val[0]);
    vadd8(v2.val[1], v2.val[1], vt1.val[1]);
    vadd8(v2.val[2], v2.val[2], vt1.val[2]);
    vadd8(v2.val[3], v2.val[3], vt1.val[3]);

    // level 3
    arrange(vt0.val[0], vt1.val[0], v0.val[0], v1.val[0]);
    arrange(vt0.val[2], vt1.val[2], v2.val[0], v3.val[0]);
    arrange(vt0.val[1], vt1.val[1], v0.val[1], v1.val[1]);
    arrange(vt0.val[3], vt1.val[3], v2.val[1], v3.val[1]);

    vloadx4(z, &pdata[_TWIST32_NTT + i]);
    fqmul(v0.val[0], vt0.val[0], z.val[0], t);
    fqmul(v2.val[0], vt0.val[2], z.val[1], t);
    fqmul(v1.val[0], vt1.val[0], z.val[2], t);
    fqmul(v3.val[0], vt1.val[2], z.val[3], t);

    vloadx4(z, &pdata[_TWIST32_NTT + i + 32]);
    fqmul(v0.val[1], vt0.val[1], z.val[0], t);
    fqmul(v2.val[1], vt0.val[3], z.val[1], t);
    fqmul(v1.val[1], vt1.val[1], z.val[2], t);
    fqmul(v3.val[1], vt1.val[3], z.val[3], t);

    arrange(vt0.val[0], vt1.val[0], v0.val[2], v1.val[2]);
    arrange(vt0.val[2], vt1.val[2], v2.val[2], v3.val[2]);
    arrange(vt0.val[1], vt1.val[1], v0.val[3], v1.val[3]);
    arrange(vt0.val[3], vt1.val[3], v2.val[3], v3.val[3]);

    vloadx4(z, &pdata[_TWIST32_NTT + i + 64]);

    fqmul(vt0.val[0], vt0.val[0], z.val[0], t);
    fqmul(vt0.val[2], vt0.val[2], z.val[1], t);
    fqmul(vt1.val[0], vt1.val[0], z.val[2], t);
    fqmul(vt1.val[2], vt1.val[2], z.val[3], t);

    vloadx4(z, &pdata[_TWIST32_NTT + i + 96]);

    fqmul(vt0.val[1], vt0.val[1], z.val[0], t);
    fqmul(vt0.val[3], vt0.val[3], z.val[1], t);
    fqmul(vt1.val[1], vt1.val[1], z.val[2], t);
    fqmul(vt1.val[3], vt1.val[3], z.val[3], t);

    vadd8(v0.val[2], v0.val[0], vt0.val[0]);
    vadd8(v2.val[2], v2.val[0], vt0.val[2]);
    vsub8(v0.val[0], v0.val[0], vt0.val[0]);
    vsub8(v2.val[0], v2.val[0], vt0.val[2]);

    vadd8(v1.val[2], v1.val[0], vt1.val[0]);
    vadd8(v3.val[2], v3.val[0], vt1.val[2]);
    vsub8(v1.val[0], v1.val[0], vt1.val[0]);
    vsub8(v3.val[0], v3.val[0], vt1.val[2]);

    vadd8(v0.val[3], v0.val[1], vt0.val[1]);
    vadd8(v2.val[3], v2.val[1], vt0.val[3]);
    vsub8(v0.val[1], v0.val[1], vt0.val[1]);
    vsub8(v2.val[1], v2.val[1], vt0.val[3]);

    vadd8(v1.val[3], v1.val[1], vt1.val[1]);
    vadd8(v3.val[3], v3.val[1], vt1.val[3]);
    vsub8(v1.val[1], v1.val[1], vt1.val[1]);
    vsub8(v3.val[1], v3.val[1], vt1.val[3]);

    // level 4
    z.val[0] = vdupq_n_s16(pdata[_ZETAS + 0]);
    z.val[3] = vld1q_s16(&pdata[_8XMONT]);

    fqmul(vt0.val[0], v0.val[3], z.val[3], t);
    fqmul(vt0.val[1], v1.val[3], z.val[3], t);
    fqmul(vt0.val[2], v2.val[3], z.val[3], t);
    fqmul(vt0.val[3], v3.val[3], z.val[3], t);

    fqmul(vt1.val[0], v0.val[1], z.val[0], t);
    fqmul(vt1.val[1], v1.val[1], z.val[0], t);
    fqmul(vt1.val[2], v2.val[1], z.val[0], t);
    fqmul(vt1.val[3], v3.val[1], z.val[0], t);

    vadd8(v0.val[3], v0.val[2], vt0.val[0]);
    vadd8(v2.val[3], v2.val[2], vt0.val[2]);
    vsub8(v0.val[2], v0.val[2], vt0.val[0]);
    vsub8(v2.val[2], v2.val[2], vt0.val[2]);

    vadd8(v1.val[3], v1.val[2], vt0.val[1]);
    vadd8(v3.val[3], v3.val[2], vt0.val[3]);
    vsub8(v1.val[2], v1.val[2], vt0.val[1]);
    vsub8(v3.val[2], v3.val[2], vt0.val[3]);

    vadd8(v0.val[1], v0.val[0], vt1.val[0]);
    vadd8(v2.val[1], v2.val[0], vt1.val[2]);
    vsub8(v0.val[0], v0.val[0], vt1.val[0]);
    vsub8(v2.val[0], v2.val[0], vt1.val[2]);

    vadd8(v1.val[1], v1.val[0], vt1.val[1]);
    vadd8(v3.val[1], v3.val[0], vt1.val[3]);
    vsub8(v1.val[0], v1.val[0], vt1.val[1]);
    vsub8(v3.val[0], v3.val[0], vt1.val[3]);

    // level 5
    z.val[1] = vdupq_n_s16(pdata[_ZETAS + 1]);
    z.val[2] = vdupq_n_s16(pdata[_ZETAS + 2]);

    fqmul(vt0.val[0], v1.val[0], z.val[2], t);
    fqmul(vt0.val[1], v1.val[1], z.val[1], t);
    fqmul(vt0.val[2], v1.val[2], z.val[0], t);
    fqmul(vt0.val[3], v1.val[3], z.val[3], t);

    fqmul(vt1.val[0], v3.val[0], z.val[2], t);
    fqmul(vt1.val[1], v3.val[1], z.val[1], t);
    fqmul(vt1.val[2], v3.val[2], z.val[0], t);
    fqmul(vt1.val[3], v3.val[3], z.val[3], t);

    vadd8(v1.val[3], v0.val[3], vt0.val[3]);
    vadd8(v3.val[3], v2.val[3], vt1.val[3]);
    vsub8(v0.val[3], v0.val[3], vt0.val[3]);
    vsub8(v2.val[3], v2.val[3], vt1.val[3]);

    vadd8(v1.val[2], v0.val[2], vt0.val[2]);
    vadd8(v3.val[2], v2.val[2], vt1.val[2]);
    vsub8(v0.val[2], v0.val[2], vt0.val[2]);
    vsub8(v2.val[2], v2.val[2], vt1.val[2]);

    vadd8(v1.val[1], v0.val[1], vt0.val[1]);
    vadd8(v3.val[1], v2.val[1], vt1.val[1]);
    vsub8(v0.val[1], v0.val[1], vt0.val[1]);
    vsub8(v2.val[1], v2.val[1], vt1.val[1]);

    vadd8(v1.val[0], v0.val[0], vt0.val[0]);
    vadd8(v3.val[0], v2.val[0], vt1.val[0]);
    vsub8(v0.val[0], v0.val[0], vt0.val[0]);
    vsub8(v2.val[0], v2.val[0], vt1.val[0]);

    // level 6
    fqmul(v1.val[3], v1.val[3], z.val[3], t);
    fqmul(v3.val[3], v3.val[3], z.val[3], t);

    vloadx4(z, &pdata[_TWIST4]);

    fqmul(v0.val[3], v0.val[3], z.val[1], t);
    fqmul(v2.val[3], v2.val[3], z.val[1], t);
    fqmul(v1.val[2], v1.val[2], z.val[2], t);
    fqmul(v3.val[2], v3.val[2], z.val[2], t);

    fqmul(v0.val[2], v0.val[2], z.val[3], t);
    fqmul(v2.val[2], v2.val[2], z.val[3], t);

    vloadx4(z, &pdata[_TWIST4 + 32]);

    fqmul(v1.val[1], v1.val[1], z.val[0], t);
    fqmul(v3.val[1], v3.val[1], z.val[0], t);
    fqmul(v0.val[1], v0.val[1], z.val[1], t);
    fqmul(v2.val[1], v2.val[1], z.val[1], t);

    fqmul(v1.val[0], v1.val[0], z.val[2], t);
    fqmul(v3.val[0], v3.val[0], z.val[2], t);
    fqmul(v0.val[0], v0.val[0], z.val[3], t);
    fqmul(v2.val[0], v2.val[0], z.val[3], t);

    // transpose 4x4
    transpose(v0.val[3], v0.val[2], v0.val[1], v0.val[0], t);
    transpose(v1.val[3], v1.val[2], v1.val[1], v1.val[0], t);
    transpose(v2.val[3], v2.val[2], v2.val[1], v2.val[0], t);
    transpose(v3.val[3], v3.val[2], v3.val[1], v3.val[0], t);

    t.val[0] = v1.val[3];
    t.val[1] = v3.val[3];
    t.val[2] = v1.val[2];
    t.val[3] = v3.val[2];

    vadd8(v1.val[3], t.val[0], v1.val[1]);
    vadd8(v3.val[3], t.val[1], v3.val[1]);
    vsub8(v1.val[1], t.val[0], v1.val[1]);
    vsub8(v3.val[1], t.val[1], v3.val[1]);

    vadd8(v1.val[2], t.val[2], v1.val[0]);
    vadd8(v3.val[2], t.val[3], v3.val[0]);
    vsub8(v1.val[0], t.val[2], v1.val[0]);
    vsub8(v3.val[0], t.val[3], v3.val[0]);

    t.val[0] = v0.val[3];
    t.val[1] = v2.val[3];
    t.val[2] = v0.val[2];
    t.val[3] = v2.val[2];

    vadd8(v0.val[3], t.val[0], v0.val[1]);
    vadd8(v2.val[3], t.val[1], v2.val[1]);
    vsub8(v0.val[1], t.val[0], v0.val[1]);
    vsub8(v2.val[1], t.val[1], v2.val[1]);

    vadd8(v0.val[2], t.val[2], v0.val[0]);
    vadd8(v2.val[2], t.val[3], v2.val[0]);
    vsub8(v0.val[0], t.val[2], v0.val[0]);
    vsub8(v2.val[0], t.val[3], v2.val[0]);

    // level 7
    z.val[0] = vdupq_n_s16(pdata[_ZETAS + 0]);
    fqmul(v0.val[0], v0.val[0], z.val[0], t);
    fqmul(v1.val[0], v1.val[0], z.val[0], t);
    fqmul(v2.val[0], v2.val[0], z.val[0], t);
    fqmul(v3.val[0], v3.val[0], z.val[0], t);

    vadd8(vt1.val[0], v1.val[3], v1.val[2]);
    vadd8(vt1.val[1], v3.val[3], v3.val[2]);
    vsub8(vt1.val[2], v1.val[3], v1.val[2]);
    vsub8(vt1.val[3], v3.val[3], v3.val[2]);

    vadd8(vt0.val[0], v1.val[1], v1.val[0]);
    vadd8(vt0.val[1], v3.val[1], v3.val[0]);
    vsub8(vt0.val[2], v1.val[1], v1.val[0]);
    vsub8(vt0.val[3], v3.val[1], v3.val[0]);

    vstorex4(&out->coeffs[i], vt1);
    vstorex4(&out->coeffs[i + 32], vt0);

    vadd8(vt1.val[0], v0.val[3], v0.val[2]);
    vadd8(vt1.val[1], v2.val[3], v2.val[2]);
    vsub8(vt1.val[2], v0.val[3], v0.val[2]);
    vsub8(vt1.val[3], v2.val[3], v2.val[2]);

    vadd8(vt0.val[0], v0.val[1], v0.val[0]);
    vadd8(vt0.val[1], v2.val[1], v2.val[0]);
    vsub8(vt0.val[2], v0.val[1], v0.val[0]);
    vsub8(vt0.val[3], v2.val[1], v2.val[0]);

    vstorex4(&out->coeffs[i + 64], vt1);
    vstorex4(&out->coeffs[i + 96], vt0);
  }
}

void neon_invntt(poly *out, const nttpoly *in, const int16_t *pdata)
{
  int i;
  int16x8x4_t v0, v1, v2, v3, t, z;
  int16x8_t neon_p, neon_pinv, neon_mont;

  neon_p = vld1q_s16(&pdata[_8XP]);
  neon_pinv = vld1q_s16(&pdata[_8XPINV]);

  // Total SIMD register: 27
  for (i = 0; i < 256; i += 128)
  {
    // level 0: distance 16
    neon_mont = vld1q_s16(&pdata[_8XMONT]);

    vloadx4(v0, &in->coeffs[i]);
    vloadx4(v1, &in->coeffs[i + 32]);
    vloadx4(v2, &in->coeffs[i + 64]);
    vloadx4(v3, &in->coeffs[i + 96]);

    subadd(v0, 0, 2, 1, 3, t, 0);
    subadd(v1, 0, 2, 1, 3, t, 2);
    subadd(v2, 0, 2, 1, 3, t, 0);
    subadd(v3, 0, 2, 1, 3, t, 2);

    z.val[0] = vdupq_n_s16(pdata[_ZETAS + 0]);

    fqmul(v1.val[2], v1.val[2], z.val[0], t);
    fqmul(v1.val[3], v1.val[3], z.val[0], t);
    fqmul(v3.val[2], v3.val[2], z.val[0], t);
    fqmul(v3.val[3], v3.val[3], z.val[0], t);

    // level 1
    // v0 x v1
    subadd_x4(v1, v0, t);

    // v2 x v3
    subadd_x4(v3, v2, t);

    transpose(v0.val[0], v1.val[2], v1.val[0], v0.val[2], t);
    transpose(v0.val[1], v1.val[3], v1.val[1], v0.val[3], t);

    transpose(v2.val[0], v3.val[2], v3.val[0], v2.val[2], t);
    transpose(v2.val[1], v3.val[3], v3.val[1], v2.val[3], t);

    fqmul(v0.val[0], v0.val[0], neon_mont, t);
    fqmul(v0.val[1], v0.val[1], neon_mont, t);

    vloadx4(z, &pdata[_TWIST4]);

    fqmul(v2.val[0], v2.val[0], z.val[0], t);
    fqmul(v2.val[1], v2.val[1], z.val[0], t);

    fqmul(v3.val[2], v3.val[2], z.val[2], t);
    fqmul(v3.val[3], v3.val[3], z.val[2], t);
    fqmul(v1.val[2], v1.val[2], z.val[3], t);
    fqmul(v1.val[3], v1.val[3], z.val[3], t);

    vloadx4(z, &pdata[_TWIST4 + 32]);

    fqmul(v2.val[2], v2.val[2], z.val[0], t);
    fqmul(v2.val[3], v2.val[3], z.val[0], t);
    fqmul(v0.val[2], v0.val[2], z.val[1], t);
    fqmul(v0.val[3], v0.val[3], z.val[1], t);

    fqmul(v3.val[0], v3.val[0], z.val[2], t);
    fqmul(v3.val[1], v3.val[1], z.val[2], t);
    fqmul(v1.val[0], v1.val[0], z.val[3], t);
    fqmul(v1.val[1], v1.val[1], z.val[3], t);

    // level 2
    vcopy(t.val[0], v3.val[0]);
    vcopy(t.val[1], v3.val[1]);
    vcopy(t.val[2], v3.val[2]);
    vcopy(t.val[3], v3.val[3]);

    vsub8(v3.val[0], v3.val[0], v1.val[0]);
    vsub8(v3.val[1], v3.val[1], v1.val[1]);
    vsub8(v3.val[2], v3.val[2], v1.val[2]);
    vsub8(v3.val[3], v3.val[3], v1.val[3]);

    vadd8(v1.val[0], v1.val[0], t.val[0]);
    vadd8(v1.val[1], v1.val[1], t.val[1]);
    vadd8(v1.val[2], v1.val[2], t.val[2]);
    vadd8(v1.val[3], v1.val[3], t.val[3]);

    // Compiler optimized this
    vcopy(t.val[0], v2.val[0]);
    vcopy(t.val[1], v2.val[1]);
    vcopy(t.val[2], v2.val[2]);
    vcopy(t.val[3], v2.val[3]);

    vsub8(v2.val[0], v0.val[0], v2.val[0]);
    vsub8(v2.val[1], v0.val[1], v2.val[1]);
    vsub8(v2.val[2], v2.val[2], v0.val[2]);
    vsub8(v2.val[3], v2.val[3], v0.val[3]);

    vadd8(v0.val[0], v0.val[0], t.val[0]);
    vadd8(v0.val[1], v0.val[1], t.val[1]);
    vadd8(v0.val[2], v0.val[2], t.val[2]);
    vadd8(v0.val[3], v0.val[3], t.val[3]);

    z.val[0] = vdupq_n_s16(pdata[_ZETAS + 0]);
    z.val[1] = vdupq_n_s16(pdata[_ZETAS + 1]);
    z.val[2] = vdupq_n_s16(pdata[_ZETAS + 2]);

    fqmul(v3.val[2], v3.val[2], z.val[0], t);
    fqmul(v3.val[3], v3.val[3], z.val[0], t);

    fqmul(v3.val[0], v3.val[0], z.val[2], t);
    fqmul(v3.val[1], v3.val[1], z.val[2], t);

    fqmul(v2.val[2], v2.val[2], z.val[1], t);
    fqmul(v2.val[3], v2.val[3], z.val[1], t);

    // level 3
    t.val[0] = v0.val[2];
    t.val[1] = v0.val[3];
    t.val[2] = v2.val[2];
    t.val[3] = v2.val[3];

    vsub8(v0.val[2], t.val[0], v1.val[0]);
    vsub8(v0.val[3], t.val[1], v1.val[1]);
    vsub8(v2.val[2], t.val[2], v3.val[0]);
    vsub8(v2.val[3], t.val[3], v3.val[1]);

    vadd8(v1.val[0], t.val[0], v1.val[0]);
    vadd8(v1.val[1], t.val[1], v1.val[1]);
    vadd8(v3.val[0], t.val[2], v3.val[0]);
    vadd8(v3.val[1], t.val[3], v3.val[1]);

    t.val[0] = v1.val[2];
    t.val[1] = v1.val[3];
    t.val[2] = v3.val[2];
    t.val[3] = v3.val[3];

    vsub8(v1.val[2], v0.val[0], t.val[0]);
    vsub8(v1.val[3], v0.val[1], t.val[1]);
    vsub8(v3.val[2], v2.val[0], t.val[2]);
    vsub8(v3.val[3], v2.val[1], t.val[3]);

    vadd8(v0.val[0], v0.val[0], t.val[0]);
    vadd8(v0.val[1], v0.val[1], t.val[1]);
    vadd8(v2.val[0], v2.val[0], t.val[2]);
    vadd8(v2.val[1], v2.val[1], t.val[3]);

    fqmul(v0.val[2], v0.val[2], z.val[0], t);
    fqmul(v0.val[3], v0.val[3], z.val[0], t);

    fqmul(v2.val[2], v2.val[2], z.val[0], t);
    fqmul(v2.val[3], v2.val[3], z.val[0], t);

    // level 4
    fqmul(v0.val[0], v0.val[0], neon_mont, t);
    fqmul(v0.val[1], v0.val[1], neon_mont, t);

    t.val[0] = v1.val[0];
    t.val[1] = v1.val[1];
    t.val[2] = v3.val[0];
    t.val[3] = v3.val[1];

    vadd8(v1.val[0], v0.val[0], t.val[0]);
    vadd8(v1.val[1], v0.val[1], t.val[1]);
    vadd8(v3.val[0], v2.val[0], t.val[2]);
    vadd8(v3.val[1], v2.val[1], t.val[3]);

    vsub8(v0.val[0], v0.val[0], t.val[0]);
    vsub8(v0.val[1], v0.val[1], t.val[1]);
    vsub8(v2.val[0], v2.val[0], t.val[2]);
    vsub8(v2.val[1], v2.val[1], t.val[3]);

    t.val[0] = v0.val[2];
    t.val[1] = v0.val[3];
    t.val[2] = v2.val[2];
    t.val[3] = v2.val[3];

    vadd8(v0.val[2], v1.val[2], t.val[0]);
    vadd8(v0.val[3], v1.val[3], t.val[1]);
    vadd8(v2.val[2], v3.val[2], t.val[2]);
    vadd8(v2.val[3], v3.val[3], t.val[3]);

    vsub8(v1.val[2], v1.val[2], t.val[0]);
    vsub8(v1.val[3], v1.val[3], t.val[1]);
    vsub8(v3.val[2], v3.val[2], t.val[2]);
    vsub8(v3.val[3], v3.val[3], t.val[3]);

    vloadx4(z, &pdata[_TWIST32_INVNTT + i]);

    fqmul(z.val[0], v1.val[0], z.val[0], t);
    fqmul(z.val[1], v1.val[1], z.val[1], t);
    fqmul(z.val[2], v3.val[0], z.val[2], t);
    fqmul(z.val[3], v3.val[1], z.val[3], t);

    arrange(v1.val[0], v3.val[0], z.val[0], z.val[2]);
    arrange(v1.val[1], v3.val[1], z.val[1], z.val[3]);

    vloadx4(z, &pdata[_TWIST32_INVNTT + i + 32]);

    fqmul(z.val[0], v0.val[2], z.val[0], t);
    fqmul(z.val[1], v0.val[3], z.val[1], t);
    fqmul(z.val[2], v2.val[2], z.val[2], t);
    fqmul(z.val[3], v2.val[3], z.val[3], t);

    arrange(v0.val[2], v2.val[2], z.val[0], z.val[2]);
    arrange(v0.val[3], v2.val[3], z.val[1], z.val[3]);

    vloadx4(z, &pdata[_TWIST32_INVNTT + i + 64]);

    fqmul(z.val[0], v0.val[0], z.val[0], t);
    fqmul(z.val[1], v0.val[1], z.val[1], t);
    fqmul(z.val[2], v2.val[0], z.val[2], t);
    fqmul(z.val[3], v2.val[1], z.val[3], t);

    arrange(v0.val[0], v2.val[0], z.val[0], z.val[2]);
    arrange(v0.val[1], v2.val[1], z.val[1], z.val[3]);

    vloadx4(z, &pdata[_TWIST32_INVNTT + i + 96]);

    fqmul(z.val[0], v1.val[2], z.val[0], t);
    fqmul(z.val[1], v1.val[3], z.val[1], t);
    fqmul(z.val[2], v3.val[2], z.val[2], t);
    fqmul(z.val[3], v3.val[3], z.val[3], t);

    arrange(v1.val[2], v3.val[2], z.val[0], z.val[2]);
    arrange(v1.val[3], v3.val[3], z.val[1], z.val[3]);

    // level 5

    z.val[0] = vdupq_n_s16(pdata[_ZETAS + 6 - (i >> 6)]);
    z.val[1] = vdupq_n_s16(pdata[_ZETAS + 5 - (i >> 6)]);

    t.val[0] = v2.val[2];
    t.val[1] = v2.val[3];
    t.val[2] = v3.val[0];
    t.val[3] = v3.val[1];

    vsub8(v2.val[2], t.val[0], v0.val[2]);
    vsub8(v2.val[3], t.val[1], v0.val[3]);
    vsub8(v3.val[0], t.val[2], v1.val[0]);
    vsub8(v3.val[1], t.val[3], v1.val[1]);

    vadd8(v0.val[2], v0.val[2], t.val[0]);
    vadd8(v0.val[3], v0.val[3], t.val[1]);
    vadd8(v1.val[0], v1.val[0], t.val[2]);
    vadd8(v1.val[1], v1.val[1], t.val[3]);

    t.val[0] = v2.val[0];
    t.val[1] = v2.val[1];
    t.val[2] = v3.val[2];
    t.val[3] = v3.val[3];

    vsub8(v2.val[0], t.val[0], v0.val[0]);
    vsub8(v2.val[1], t.val[1], v0.val[1]);
    vsub8(v3.val[2], t.val[2], v1.val[2]);
    vsub8(v3.val[3], t.val[3], v1.val[3]);

    vadd8(v0.val[0], v0.val[0], t.val[0]);
    vadd8(v0.val[1], v0.val[1], t.val[1]);
    vadd8(v1.val[2], v1.val[2], t.val[2]);
    vadd8(v1.val[3], v1.val[3], t.val[3]);

    fqmul(v2.val[2], v2.val[2], z.val[0], t);
    fqmul(v2.val[3], v2.val[3], z.val[1], t);

    fqmul(v3.val[0], v3.val[0], z.val[0], t);
    fqmul(v3.val[1], v3.val[1], z.val[1], t);

    fqmul(v2.val[0], v2.val[0], z.val[0], t);
    fqmul(v2.val[1], v2.val[1], z.val[1], t);

    fqmul(v3.val[2], v3.val[2], z.val[0], t);
    fqmul(v3.val[3], v3.val[3], z.val[1], t);

    // level 6
    t.val[0] = v1.val[1];
    t.val[1] = v0.val[3];
    t.val[2] = v0.val[1];
    t.val[3] = v1.val[3];

    vsub8(v1.val[1], t.val[0], v1.val[0]);
    vsub8(v0.val[3], t.val[1], v0.val[2]);
    vsub8(v0.val[1], t.val[2], v0.val[0]);
    vsub8(v1.val[3], t.val[3], v1.val[2]);

    vadd8(v1.val[0], v1.val[0], t.val[0]);
    vadd8(v0.val[2], v0.val[2], t.val[1]);
    vadd8(v0.val[0], v0.val[0], t.val[2]);
    vadd8(v1.val[2], v1.val[2], t.val[3]);

    fqmul(z.val[2], v0.val[0], neon_mont, t); // 2
    fqmul(z.val[1], v0.val[2], neon_mont, t); // 1
    fqmul(z.val[0], v1.val[0], neon_mont, t); // 0
    fqmul(z.val[3], v1.val[2], neon_mont, t); // 3

    vstorex4(&out->coeffs[i], z);

    t.val[0] = v3.val[1];
    t.val[1] = v2.val[3];
    t.val[2] = v2.val[1];
    t.val[3] = v3.val[3];

    vsub8(v3.val[1], t.val[0], v3.val[0]);
    vsub8(v2.val[3], t.val[1], v2.val[2]);
    vsub8(v2.val[1], t.val[2], v2.val[0]);
    vsub8(v3.val[3], t.val[3], v3.val[2]);

    vadd8(z.val[0], v3.val[0], t.val[0]);
    vadd8(z.val[1], v2.val[2], t.val[1]);
    vadd8(z.val[2], v2.val[0], t.val[2]);
    vadd8(z.val[3], v3.val[2], t.val[3]);

    vstorex4(&out->coeffs[i + 32], z);

    neon_mont = vdupq_n_s16(pdata[_ZETAS + 2 - (i >> 7)]);

    fqmul(z.val[2], v0.val[1], neon_mont, t); // 2
    fqmul(z.val[1], v0.val[3], neon_mont, t); // 1
    fqmul(z.val[0], v1.val[1], neon_mont, t); // 0
    fqmul(z.val[3], v1.val[3], neon_mont, t); // 3

    vstorex4(&out->coeffs[i + 64], z);

    fqmul(z.val[2], v2.val[1], neon_mont, t); // 2
    fqmul(z.val[1], v2.val[3], neon_mont, t); // 1
    fqmul(z.val[0], v3.val[1], neon_mont, t); // 0
    fqmul(z.val[3], v3.val[3], neon_mont, t); // 3

    vstorex4(&out->coeffs[i + 96], z);
  }

  z.val[0] = vdupq_n_s16(pdata[_ZETAS + 0]);

  for (i = 0; i < 128; i += 64)
  {
    vloadx4(v0, &out->coeffs[i]);
    vloadx4(v1, &out->coeffs[i + 32]);
    vloadx4(v2, &out->coeffs[i + 128]);
    vloadx4(v3, &out->coeffs[i + 160]);

    // v0 x v2
    t.val[0] = v2.val[0];
    t.val[1] = v2.val[1];
    t.val[2] = v2.val[2];
    t.val[3] = v2.val[3];
    vsub8(v2.val[0], t.val[0], v0.val[0]);
    vsub8(v2.val[1], t.val[1], v0.val[1]);
    vsub8(v2.val[2], t.val[2], v0.val[2]);
    vsub8(v2.val[3], t.val[3], v0.val[3]);
    vadd8(v0.val[0], t.val[0], v0.val[0]);
    vadd8(v0.val[1], t.val[1], v0.val[1]);
    vadd8(v0.val[2], t.val[2], v0.val[2]);
    vadd8(v0.val[3], t.val[3], v0.val[3]);

    // v1 x v3
    t.val[0] = v3.val[0];
    t.val[1] = v3.val[1];
    t.val[2] = v3.val[2];
    t.val[3] = v3.val[3];
    vsub8(v3.val[0], t.val[0], v1.val[0]);
    vsub8(v3.val[1], t.val[1], v1.val[1]);
    vsub8(v3.val[2], t.val[2], v1.val[2]);
    vsub8(v3.val[3], t.val[3], v1.val[3]);
    vadd8(v1.val[0], t.val[0], v1.val[0]);
    vadd8(v1.val[1], t.val[1], v1.val[1]);
    vadd8(v1.val[2], t.val[2], v1.val[2]);
    vadd8(v1.val[3], t.val[3], v1.val[3]);

    fqmul(v2.val[0], v2.val[0], z.val[0], t);
    fqmul(v2.val[1], v2.val[1], z.val[0], t);
    fqmul(v2.val[2], v2.val[2], z.val[0], t);
    fqmul(v2.val[3], v2.val[3], z.val[0], t);

    fqmul(v3.val[0], v3.val[0], z.val[0], t);
    fqmul(v3.val[1], v3.val[1], z.val[0], t);
    fqmul(v3.val[2], v3.val[2], z.val[0], t);
    fqmul(v3.val[3], v3.val[3], z.val[0], t);

    vstorex4(&out->coeffs[i], v0);
    vstorex4(&out->coeffs[i + 32], v1);
    vstorex4(&out->coeffs[i + 128], v2);
    vstorex4(&out->coeffs[i + 160], v3);
  }
}
