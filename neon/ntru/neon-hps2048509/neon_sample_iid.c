/*=============================================================================
 * Copyright (c) 2020 by Cryptographic Engineering Research Group (CERG)
 * ECE Department, George Mason University
 * Fairfax, VA, U.S.A.
 * Author: Duc Tri Nguyen

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

=============================================================================*/
#include <arm_neon.h>
#include "sample.h"

#if defined(__clang__)

// load c <= a
#define sp_vload(c, a) c = vld1q_u16_x4(a);

#elif defined(__GNUC__)

// load c <= a
#define sp_vload(c, a)          \
    c.val[0] = vld1q_u16(a);      \
    c.val[1] = vld1q_u16(a + 8);  \
    c.val[2] = vld1q_u16(a + 16); \
    c.val[3] = vld1q_u16(a + 24);

#else
#error "Unsupported compiler"
#endif


// store c <= a
#define sp_vstore(c, a)                       \
    vst1q_u16(c +  0, (uint16x8_t) a.val[0]); \
    vst1q_u16(c +  8, (uint16x8_t) a.val[1]); \
    vst1q_u16(c + 16, (uint16x8_t) a.val[2]); \
    vst1q_u16(c + 24, (uint16x8_t) a.val[3]);

// c = a >> value
#define sp_vsr(c, a, value)                  \
    c.val[0] = vshrq_n_u16(a.val[0], value); \
    c.val[1] = vshrq_n_u16(a.val[1], value); \
    c.val[2] = vshrq_n_u16(a.val[2], value); \
    c.val[3] = vshrq_n_u16(a.val[3], value);

// c = a >> value
#define sp_vsr_sign(c, a, value)             \
    c.val[0] = vshrq_n_s16(a.val[0], value); \
    c.val[1] = vshrq_n_s16(a.val[1], value); \
    c.val[2] = vshrq_n_s16(a.val[2], value); \
    c.val[3] = vshrq_n_s16(a.val[3], value);

// c = a << value
#define sp_vsl(c, a, value)                  \
    c.val[0] = vshlq_n_u16(a.val[0], value); \
    c.val[1] = vshlq_n_u16(a.val[1], value); \
    c.val[2] = vshlq_n_u16(a.val[2], value); \
    c.val[3] = vshlq_n_u16(a.val[3], value);

// c = a & const
#define sp_vand_const(c, a, b)         \
    c.val[0] = vandq_u16(a.val[0], b); \
    c.val[1] = vandq_u16(a.val[1], b); \
    c.val[2] = vandq_u16(a.val[2], b); \
    c.val[3] = vandq_u16(a.val[3], b);

// c = a & b
#define sp_vand_sign(c, a, b)                 \
    c.val[0] = vandq_s16(a.val[0], (int16x8_t) b.val[0]); \
    c.val[1] = vandq_s16(a.val[1], (int16x8_t) b.val[1]); \
    c.val[2] = vandq_s16(a.val[2], (int16x8_t) b.val[2]); \
    c.val[3] = vandq_s16(a.val[3], (int16x8_t) b.val[3]);

// c = a + b
#define sp_vadd(c, a, b)                      \
    c.val[0] = vaddq_u16(a.val[0], b.val[0]); \
    c.val[1] = vaddq_u16(a.val[1], b.val[1]); \
    c.val[2] = vaddq_u16(a.val[2], b.val[2]); \
    c.val[3] = vaddq_u16(a.val[3], b.val[3]);

// c = a - b
#define sp_vsub_const_sign(c, a, b)    \
    c.val[0] = vsubq_s16( (int16x8_t) a.val[0], (int16x8_t) b); \
    c.val[1] = vsubq_s16( (int16x8_t) a.val[1], (int16x8_t) b); \
    c.val[2] = vsubq_s16( (int16x8_t) a.val[2], (int16x8_t) b); \
    c.val[3] = vsubq_s16( (int16x8_t) a.val[3], (int16x8_t) b);

// c = a - const
#define sp_vsub_const(c, a, b)         \
    c.val[0] = vsubq_u16(a.val[0], b); \
    c.val[1] = vsubq_u16(a.val[1], b); \
    c.val[2] = vsubq_u16(a.val[2], b); \
    c.val[3] = vsubq_u16(a.val[3], b);

// c = a ^ b
#define sp_vxor_sign(c, a, b)                 \
    c.val[0] = veorq_s16(a.val[0], b.val[0]); \
    c.val[1] = veorq_s16(a.val[1], b.val[1]); \
    c.val[2] = veorq_s16(a.val[2], b.val[2]); \
    c.val[3] = veorq_s16(a.val[3], b.val[3]);

// c = a ^ b
#define sp_vxor_const(c, a, b)         \
    c.val[0] = veorq_u16(a.val[0], b); \
    c.val[1] = veorq_u16(a.val[1], b); \
    c.val[2] = veorq_u16(a.val[2], b); \
    c.val[3] = veorq_u16(a.val[3], b);

// c = ~a
#define sp_vnot_sign(c, a)          \
    c.val[0] = vmvnq_s16(a.val[0]); \
    c.val[1] = vmvnq_s16(a.val[1]); \
    c.val[2] = vmvnq_s16(a.val[2]); \
    c.val[3] = vmvnq_s16(a.val[3]);

// c = value
#define sp_vdup_x1(c, value) c = vdupq_n_u16(value);


void sample_iid(poly *r, const unsigned char uniformbytes[NTRU_SAMPLE_IID_BYTES])
{
    // 35 SIMD registers
    uint16x8x4_t r1, r2, r3;
    uint8x16x2_t r_buf;
    int16x8x4_t t, c, a, b;
    uint16x8_t hex_0x03, hex_0x0f, hex_0xff;
    sp_vdup_x1(hex_0x03, 0x03);
    sp_vdup_x1(hex_0xff, 0xff);
    sp_vdup_x1(hex_0x0f, 0x0f);

    for (uint16_t addr = 0; addr < NTRU_N_32; addr += 32)
    {
        r_buf = vld1q_u8_x2(&uniformbytes[addr]);
        r3.val[0] = vshll_n_u8(vget_low_u8(r_buf.val[0]), 0);
        r3.val[1] = vshll_high_n_u8(r_buf.val[0], 0);
        r3.val[2] = vshll_n_u8(vget_low_u8(r_buf.val[1]), 0);
        r3.val[3] = vshll_high_n_u8(r_buf.val[1], 0);
 
        // r3 = (res >> 8) + (res & 0xff)
        sp_vsr(r1, r3, 8);
        sp_vand_const(r2, r3, hex_0xff);
        sp_vadd(r3, r1, r2);

        // r3 = (r3 >> 4) + (r3 & 0xf)
        sp_vsr(r1, r3, 4);
        sp_vand_const(r2, r3, hex_0x0f);
        sp_vadd(r3, r1, r2);

        // r3 = (r3 >> 2) + (r3 & 0x3)
        sp_vsr(r1, r3, 2);
        sp_vand_const(r2, r3, hex_0x03);
        sp_vadd(r3, r1, r2);

        // r3 = (r3 >> 2) + (r3 & 0x3)
        sp_vsr(r1, r3, 2);
        sp_vand_const(r2, r3, hex_0x03);
        sp_vadd(r3, r1, r2);

        // t = r3 - 3
        sp_vsub_const_sign(t, r3, hex_0x03);
        // c = t >> 15
        sp_vsr_sign(c, t, 15);

        // a = c & r3
        sp_vand_sign(a, c, r3);
        // b = ~c & t
        sp_vnot_sign(b, c);
        sp_vand_sign(b, b, t);
        // c = a ^ b
        sp_vxor_sign(c, a, b);

        sp_vstore(&r->coeffs[addr], c);
    }
    r->coeffs[NTRU_N - 1] = 0;
}
