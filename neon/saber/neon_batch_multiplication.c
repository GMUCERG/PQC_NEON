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

// c = aa * bb
#define sb_vmul(c, aa, bb) c = vmulq_u16(aa, bb);

// c += aa*bb
#define sb_vmla(c, aa, bb) c = vmlaq_u16(c, aa, bb);

// load c <= a
#define sb_vload(c, a) c = vld1q_u16(a);

// store c <= a
#define sb_vstore(c, a) vst1q_u16(c, a);

// c = aa ^ bb
#define sb_vxor(c, aa, bb) c = veorq_u16(aa, bb);

/*
=========================================
Schoolbook for left over polynomials
Input Before Transpose
  0   1   2   3   4   5   6   7   8   9  10  11  12  13  14  15
  0   1   2   3   4   5   6   7   8   9  10  11  12  13  14  15
  0   1   2   3   4   5   6   7   8   9  10  11  12  13  14  15
  0   1   2   3   4   5   6   7   8   9  10  11  12  13  14  15
  0   1   2   3   4   5   6   7   8   9  10  11  12  13  14  15
  0   1   2   3   4   5   6   7   8   9  10  11  12  13  14  15
  0   1   2   3   4   5   6   7   8   9  10  11  12  13  14  15
  0   1   2   3   4   5   6   7   8   9  10  11  12  13  14  15
Input After Transpose
Input: a_in_mem[128], b_in_mem[128]
  0   0   0   0   0   0   0   0  |  8   8   8   8   8   8   8   8
  1   1   1   1   1   1   1   1  |  9   9   9   9   9   9   9   9
  2   2   2   2   2   2   2   2  | 10  10  10  10  10  10  10  10
  3   3   3   3   3   3   3   3  | 11  11  11  11  11  11  11  11
  4   4   4   4   4   4   4   4  | 12  12  12  12  12  12  12  12
  5   5   5   5   5   5   5   5  | 13  13  13  13  13  13  13  13
  6   6   6   6   6   6   6   6  | 14  14  14  14  14  14  14  14
  7   7   7   7   7   7   7   7  | 15  15  15  15  15  15  15  15
-----------
Output: c_in_mem[256]
  0   0   0   0   0   0   0   0  |   8   8   8   8   8   8   8   8  |  16   16   16   16   16   16   16   16  |  24   24   24   24   24   24   24   24
  1   1   1   1   1   1   1   1  |   9   9   9   9   9   9   9   9  |  17   17   17   17   17   17   17   17  |  25   25   25   25   25   25   25   25
  2   2   2   2   2   2   2   2  |  10  10  10  10  10  10  10  10  |  18   18   18   18   18   18   18   18  |  26   26   26   26   26   26   26   26
  3   3   3   3   3   3   3   3  |  11  11  11  11  11  11  11  11  |  19   19   19   19   19   19   19   19  |  27   27   27   27   27   27   27   27
  4   4   4   4   4   4   4   4  |  12  12  12  12  12  12  12  12  |  20   20   20   20   20   20   20   20  |  28   28   28   28   28   28   28   28
  5   5   5   5   5   5   5   5  |  13  13  13  13  13  13  13  13  |  21   21   21   21   21   21   21   21  |  29   29   29   29   29   29   29   29
  6   6   6   6   6   6   6   6  |  14  14  14  14  14  14  14  14  |  22   22   22   22   22   22   22   22  |  30   30   30   30   30   30   30   30
  7   7   7   7   7   7   7   7  |  15  15  15  15  15  15  15  15  |  23   23   23   23   23   23   23   23  |   x    x    x    x    x    x    x    x
Output After Transpose

  0   1   2   3   4   5   6   7  |   8   9  10  11  12  13  14  15  |  16   17   18   19   20   21   22   23  |  24   25   26   27   28   29   30    x
  0   1   2   3   4   5   6   7  |   8   9  10  11  12  13  14  15  |  16   17   18   19   20   21   22   23  |  24   25   26   27   28   29   30    x
  0   1   2   3   4   5   6   7  |   8   9  10  11  12  13  14  15  |  16   17   18   19   20   21   22   23  |  24   25   26   27   28   29   30    x
  0   1   2   3   4   5   6   7  |   8   9  10  11  12  13  14  15  |  16   17   18   19   20   21   22   23  |  24   25   26   27   28   29   30    x
  0   1   2   3   4   5   6   7  |   8   9  10  11  12  13  14  15  |  16   17   18   19   20   21   22   23  |  24   25   26   27   28   29   30    x
  0   1   2   3   4   5   6   7  |   8   9  10  11  12  13  14  15  |  16   17   18   19   20   21   22   23  |  24   25   26   27   28   29   30    x
  0   1   2   3   4   5   6   7  |   8   9  10  11  12  13  14  15  |  16   17   18   19   20   21   22   23  |  24   25   26   27   28   29   30    x
  0   1   2   3   4   5   6   7  |   8   9  10  11  12  13  14  15  |  16   17   18   19   20   21   22   23  |  24   25   26   27   28   29   30    x
------
*/
void schoolbook16x16(uint16_t *restrict c_in_mem,
                     uint16_t *restrict a_in_mem,
                     uint16_t *restrict b_in_mem,
                     const unsigned int iter)
{
    uint16x8_t tmp, aa[16], bb[16];
    uint16_t *a_mem = a_in_mem, *b_mem = b_in_mem, *c_mem = c_in_mem;

    // Total SIMD registers: 33
    for (uint16_t i = 0; i < iter; i++)
    {
        sb_vload(aa[0], &a_mem[0 * 16]);
        sb_vload(bb[0], &b_mem[0 * 16]);

        sb_vmul(tmp, aa[0], bb[0]);
        sb_vstore(&c_mem[32 * 0], tmp);
        //----
        sb_vload(aa[1], &a_mem[1 * 16]);
        sb_vload(bb[1], &b_mem[1 * 16]);

        sb_vmul(tmp, aa[0], bb[1]);
        sb_vmla(tmp, aa[1], bb[0]);
        sb_vstore(&c_mem[32 * 1], tmp);
        //----
        sb_vload(aa[2], &a_mem[2 * 16]);
        sb_vload(bb[2], &b_mem[2 * 16]);

        sb_vmul(tmp, aa[0], bb[2]);
        sb_vmla(tmp, aa[1], bb[1]);
        sb_vmla(tmp, aa[2], bb[0]);
        sb_vstore(&c_mem[32 * 2], tmp);
        //----
        sb_vload(aa[3], &a_mem[3 * 16]);
        sb_vload(bb[3], &b_mem[3 * 16]);

        sb_vmul(tmp, aa[0], bb[3]);
        sb_vmla(tmp, aa[1], bb[2]);
        sb_vmla(tmp, aa[2], bb[1]);
        sb_vmla(tmp, aa[3], bb[0]);
        sb_vstore(&c_mem[32 * 3], tmp);
        //----
        sb_vload(aa[4], &a_mem[4 * 16]);
        sb_vload(bb[4], &b_mem[4 * 16]);

        sb_vmul(tmp, aa[0], bb[4]);
        sb_vmla(tmp, aa[1], bb[3]);
        sb_vmla(tmp, aa[2], bb[2]);
        sb_vmla(tmp, aa[3], bb[1]);
        sb_vmla(tmp, aa[4], bb[0]);
        sb_vstore(&c_mem[32 * 4], tmp);
        //----
        sb_vload(aa[5], &a_mem[5 * 16]);
        sb_vload(bb[5], &b_mem[5 * 16]);

        sb_vmul(tmp, aa[0], bb[5]);
        sb_vmla(tmp, aa[1], bb[4]);
        sb_vmla(tmp, aa[2], bb[3]);
        sb_vmla(tmp, aa[3], bb[2]);
        sb_vmla(tmp, aa[4], bb[1]);
        sb_vmla(tmp, aa[5], bb[0]);
        sb_vstore(&c_mem[32 * 5], tmp);
        //----
        sb_vload(aa[6], &a_mem[6 * 16]);
        sb_vload(bb[6], &b_mem[6 * 16]);

        sb_vmul(tmp, aa[0], bb[6]);
        sb_vmla(tmp, aa[1], bb[5]);
        sb_vmla(tmp, aa[2], bb[4]);
        sb_vmla(tmp, aa[3], bb[3]);
        sb_vmla(tmp, aa[4], bb[2]);
        sb_vmla(tmp, aa[5], bb[1]);
        sb_vmla(tmp, aa[6], bb[0]);
        sb_vstore(&c_mem[32 * 6], tmp);
        //----
        sb_vload(aa[7], &a_mem[7 * 16]);
        sb_vload(bb[7], &b_mem[7 * 16]);

        sb_vmul(tmp, aa[0], bb[7]);
        sb_vmla(tmp, aa[1], bb[6]);
        sb_vmla(tmp, aa[2], bb[5]);
        sb_vmla(tmp, aa[3], bb[4]);
        sb_vmla(tmp, aa[4], bb[3]);
        sb_vmla(tmp, aa[5], bb[2]);
        sb_vmla(tmp, aa[6], bb[1]);
        sb_vmla(tmp, aa[7], bb[0]);
        sb_vstore(&c_mem[32 * 7], tmp);
        //----
        sb_vload(aa[8], &a_mem[0 * 16 + 8]);
        sb_vload(bb[8], &b_mem[0 * 16 + 8]);

        sb_vmul(tmp, aa[0], bb[8]);
        sb_vmla(tmp, aa[1], bb[7]);
        sb_vmla(tmp, aa[2], bb[6]);
        sb_vmla(tmp, aa[3], bb[5]);
        sb_vmla(tmp, aa[4], bb[4]);
        sb_vmla(tmp, aa[5], bb[3]);
        sb_vmla(tmp, aa[6], bb[2]);
        sb_vmla(tmp, aa[7], bb[1]);
        sb_vmla(tmp, aa[8], bb[0]);
        sb_vstore(&c_mem[32 * 0 + 8], tmp);
        //----
        sb_vload(aa[9], &a_mem[1 * 16 + 8]);
        sb_vload(bb[9], &b_mem[1 * 16 + 8]);

        sb_vmul(tmp, aa[0], bb[9]);
        sb_vmla(tmp, aa[1], bb[8]);
        sb_vmla(tmp, aa[2], bb[7]);
        sb_vmla(tmp, aa[3], bb[6]);
        sb_vmla(tmp, aa[4], bb[5]);
        sb_vmla(tmp, aa[5], bb[4]);
        sb_vmla(tmp, aa[6], bb[3]);
        sb_vmla(tmp, aa[7], bb[2]);
        sb_vmla(tmp, aa[8], bb[1]);
        sb_vmla(tmp, aa[9], bb[0]);
        sb_vstore(&c_mem[32 * 1 + 8], tmp);
        //----
        sb_vload(aa[10], &a_mem[2 * 16 + 8]);
        sb_vload(bb[10], &b_mem[2 * 16 + 8]);

        sb_vmul(tmp, aa[0], bb[10]);
        sb_vmla(tmp, aa[1], bb[9]);
        sb_vmla(tmp, aa[2], bb[8]);
        sb_vmla(tmp, aa[3], bb[7]);
        sb_vmla(tmp, aa[4], bb[6]);
        sb_vmla(tmp, aa[5], bb[5]);
        sb_vmla(tmp, aa[6], bb[4]);
        sb_vmla(tmp, aa[7], bb[3]);
        sb_vmla(tmp, aa[8], bb[2]);
        sb_vmla(tmp, aa[9], bb[1]);
        sb_vmla(tmp, aa[10], bb[0]);
        sb_vstore(&c_mem[32 * 2 + 8], tmp);
        //----
        sb_vload(aa[11], &a_mem[3 * 16 + 8]);
        sb_vload(bb[11], &b_mem[3 * 16 + 8]);

        sb_vmul(tmp, aa[0], bb[11]);
        sb_vmla(tmp, aa[1], bb[10]);
        sb_vmla(tmp, aa[2], bb[9]);
        sb_vmla(tmp, aa[3], bb[8]);
        sb_vmla(tmp, aa[4], bb[7]);
        sb_vmla(tmp, aa[5], bb[6]);
        sb_vmla(tmp, aa[6], bb[5]);
        sb_vmla(tmp, aa[7], bb[4]);
        sb_vmla(tmp, aa[8], bb[3]);
        sb_vmla(tmp, aa[9], bb[2]);
        sb_vmla(tmp, aa[10], bb[1]);
        sb_vmla(tmp, aa[11], bb[0]);
        sb_vstore(&c_mem[32 * 3 + 8], tmp);
        //----
        sb_vload(aa[12], &a_mem[4 * 16 + 8]);
        sb_vload(bb[12], &b_mem[4 * 16 + 8]);

        sb_vmul(tmp, aa[0], bb[12]);
        sb_vmla(tmp, aa[1], bb[11]);
        sb_vmla(tmp, aa[2], bb[10]);
        sb_vmla(tmp, aa[3], bb[9]);
        sb_vmla(tmp, aa[4], bb[8]);
        sb_vmla(tmp, aa[5], bb[7]);
        sb_vmla(tmp, aa[6], bb[6]);
        sb_vmla(tmp, aa[7], bb[5]);
        sb_vmla(tmp, aa[8], bb[4]);
        sb_vmla(tmp, aa[9], bb[3]);
        sb_vmla(tmp, aa[10], bb[2]);
        sb_vmla(tmp, aa[11], bb[1]);
        sb_vmla(tmp, aa[12], bb[0]);
        sb_vstore(&c_mem[32 * 4 + 8], tmp);
        //----
        sb_vload(aa[13], &a_mem[5 * 16 + 8]);
        sb_vload(bb[13], &b_mem[5 * 16 + 8]);

        sb_vmul(tmp, aa[0], bb[13]);
        sb_vmla(tmp, aa[1], bb[12]);
        sb_vmla(tmp, aa[2], bb[11]);
        sb_vmla(tmp, aa[3], bb[10]);
        sb_vmla(tmp, aa[4], bb[9]);
        sb_vmla(tmp, aa[5], bb[8]);
        sb_vmla(tmp, aa[6], bb[7]);
        sb_vmla(tmp, aa[7], bb[6]);
        sb_vmla(tmp, aa[8], bb[5]);
        sb_vmla(tmp, aa[9], bb[4]);
        sb_vmla(tmp, aa[10], bb[3]);
        sb_vmla(tmp, aa[11], bb[2]);
        sb_vmla(tmp, aa[12], bb[1]);
        sb_vmla(tmp, aa[13], bb[0]);
        sb_vstore(&c_mem[32 * 5 + 8], tmp);
        //----
        sb_vload(aa[14], &a_mem[6 * 16 + 8]);
        sb_vload(bb[14], &b_mem[6 * 16 + 8]);

        sb_vmul(tmp, aa[0], bb[14]);
        sb_vmla(tmp, aa[1], bb[13]);
        sb_vmla(tmp, aa[2], bb[12]);
        sb_vmla(tmp, aa[3], bb[11]);
        sb_vmla(tmp, aa[4], bb[10]);
        sb_vmla(tmp, aa[5], bb[9]);
        sb_vmla(tmp, aa[6], bb[8]);
        sb_vmla(tmp, aa[7], bb[7]);
        sb_vmla(tmp, aa[8], bb[6]);
        sb_vmla(tmp, aa[9], bb[5]);
        sb_vmla(tmp, aa[10], bb[4]);
        sb_vmla(tmp, aa[11], bb[3]);
        sb_vmla(tmp, aa[12], bb[2]);
        sb_vmla(tmp, aa[13], bb[1]);
        sb_vmla(tmp, aa[14], bb[0]);
        sb_vstore(&c_mem[32 * 6 + 8], tmp);
        //----
        sb_vload(aa[15], &a_mem[7 * 16 + 8]);
        sb_vload(bb[15], &b_mem[7 * 16 + 8]);

        sb_vmul(tmp, aa[0], bb[15]);
        sb_vmla(tmp, aa[1], bb[14]);
        sb_vmla(tmp, aa[2], bb[13]);
        sb_vmla(tmp, aa[3], bb[12]);
        sb_vmla(tmp, aa[4], bb[11]);
        sb_vmla(tmp, aa[5], bb[10]);
        sb_vmla(tmp, aa[6], bb[9]);
        sb_vmla(tmp, aa[7], bb[8]);
        sb_vmla(tmp, aa[8], bb[7]);
        sb_vmla(tmp, aa[9], bb[6]);
        sb_vmla(tmp, aa[10], bb[5]);
        sb_vmla(tmp, aa[11], bb[4]);
        sb_vmla(tmp, aa[12], bb[3]);
        sb_vmla(tmp, aa[13], bb[2]);
        sb_vmla(tmp, aa[14], bb[1]);
        sb_vmla(tmp, aa[15], bb[0]);
        sb_vstore(&c_mem[32 * 7 + 8], tmp);
        //----
        //=======
        // ci =  {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15} 16
        // ai =  {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15} 16
        // bi =  {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15} 16
        // ----------------PART 2----------------
        sb_vmul(tmp, aa[1], bb[15]);
        sb_vmla(tmp, aa[2], bb[14]);
        sb_vmla(tmp, aa[3], bb[13]);
        sb_vmla(tmp, aa[4], bb[12]);
        sb_vmla(tmp, aa[5], bb[11]);
        sb_vmla(tmp, aa[6], bb[10]);
        sb_vmla(tmp, aa[7], bb[9]);
        sb_vmla(tmp, aa[8], bb[8]);
        sb_vmla(tmp, aa[9], bb[7]);
        sb_vmla(tmp, aa[10], bb[6]);
        sb_vmla(tmp, aa[11], bb[5]);
        sb_vmla(tmp, aa[12], bb[4]);
        sb_vmla(tmp, aa[13], bb[3]);
        sb_vmla(tmp, aa[14], bb[2]);
        sb_vmla(tmp, aa[15], bb[1]);
        sb_vstore(&c_mem[32 * 0 + 16], tmp);
        //-----
        sb_vmul(tmp, aa[2], bb[15]);
        sb_vmla(tmp, aa[3], bb[14]);
        sb_vmla(tmp, aa[4], bb[13]);
        sb_vmla(tmp, aa[5], bb[12]);
        sb_vmla(tmp, aa[6], bb[11]);
        sb_vmla(tmp, aa[7], bb[10]);
        sb_vmla(tmp, aa[8], bb[9]);
        sb_vmla(tmp, aa[9], bb[8]);
        sb_vmla(tmp, aa[10], bb[7]);
        sb_vmla(tmp, aa[11], bb[6]);
        sb_vmla(tmp, aa[12], bb[5]);
        sb_vmla(tmp, aa[13], bb[4]);
        sb_vmla(tmp, aa[14], bb[3]);
        sb_vmla(tmp, aa[15], bb[2]);
        sb_vstore(&c_mem[32 * 1 + 16], tmp);
        //-----
        sb_vmul(tmp, aa[3], bb[15]);
        sb_vmla(tmp, aa[4], bb[14]);
        sb_vmla(tmp, aa[5], bb[13]);
        sb_vmla(tmp, aa[6], bb[12]);
        sb_vmla(tmp, aa[7], bb[11]);
        sb_vmla(tmp, aa[8], bb[10]);
        sb_vmla(tmp, aa[9], bb[9]);
        sb_vmla(tmp, aa[10], bb[8]);
        sb_vmla(tmp, aa[11], bb[7]);
        sb_vmla(tmp, aa[12], bb[6]);
        sb_vmla(tmp, aa[13], bb[5]);
        sb_vmla(tmp, aa[14], bb[4]);
        sb_vmla(tmp, aa[15], bb[3]);
        sb_vstore(&c_mem[32 * 2 + 16], tmp);
        //-----
        sb_vmul(tmp, aa[4], bb[15]);
        sb_vmla(tmp, aa[5], bb[14]);
        sb_vmla(tmp, aa[6], bb[13]);
        sb_vmla(tmp, aa[7], bb[12]);
        sb_vmla(tmp, aa[8], bb[11]);
        sb_vmla(tmp, aa[9], bb[10]);
        sb_vmla(tmp, aa[10], bb[9]);
        sb_vmla(tmp, aa[11], bb[8]);
        sb_vmla(tmp, aa[12], bb[7]);
        sb_vmla(tmp, aa[13], bb[6]);
        sb_vmla(tmp, aa[14], bb[5]);
        sb_vmla(tmp, aa[15], bb[4]);
        sb_vstore(&c_mem[32 * 3 + 16], tmp);
        //-----
        sb_vmul(tmp, aa[5], bb[15]);
        sb_vmla(tmp, aa[6], bb[14]);
        sb_vmla(tmp, aa[7], bb[13]);
        sb_vmla(tmp, aa[8], bb[12]);
        sb_vmla(tmp, aa[9], bb[11]);
        sb_vmla(tmp, aa[10], bb[10]);
        sb_vmla(tmp, aa[11], bb[9]);
        sb_vmla(tmp, aa[12], bb[8]);
        sb_vmla(tmp, aa[13], bb[7]);
        sb_vmla(tmp, aa[14], bb[6]);
        sb_vmla(tmp, aa[15], bb[5]);
        sb_vstore(&c_mem[32 * 4 + 16], tmp);
        //-----
        sb_vmul(tmp, aa[6], bb[15]);
        sb_vmla(tmp, aa[7], bb[14]);
        sb_vmla(tmp, aa[8], bb[13]);
        sb_vmla(tmp, aa[9], bb[12]);
        sb_vmla(tmp, aa[10], bb[11]);
        sb_vmla(tmp, aa[11], bb[10]);
        sb_vmla(tmp, aa[12], bb[9]);
        sb_vmla(tmp, aa[13], bb[8]);
        sb_vmla(tmp, aa[14], bb[7]);
        sb_vmla(tmp, aa[15], bb[6]);
        sb_vstore(&c_mem[32 * 5 + 16], tmp);
        //-----
        sb_vmul(tmp, aa[7], bb[15]);
        sb_vmla(tmp, aa[8], bb[14]);
        sb_vmla(tmp, aa[9], bb[13]);
        sb_vmla(tmp, aa[10], bb[12]);
        sb_vmla(tmp, aa[11], bb[11]);
        sb_vmla(tmp, aa[12], bb[10]);
        sb_vmla(tmp, aa[13], bb[9]);
        sb_vmla(tmp, aa[14], bb[8]);
        sb_vmla(tmp, aa[15], bb[7]);
        sb_vstore(&c_mem[32 * 6 + 16], tmp);
        //-----
        sb_vmul(tmp, aa[8], bb[15]);
        sb_vmla(tmp, aa[9], bb[14]);
        sb_vmla(tmp, aa[10], bb[13]);
        sb_vmla(tmp, aa[11], bb[12]);
        sb_vmla(tmp, aa[12], bb[11]);
        sb_vmla(tmp, aa[13], bb[10]);
        sb_vmla(tmp, aa[14], bb[9]);
        sb_vmla(tmp, aa[15], bb[8]);
        sb_vstore(&c_mem[32 * 7 + 16], tmp);
        //-----
        sb_vmul(tmp, aa[9], bb[15]);
        sb_vmla(tmp, aa[10], bb[14]);
        sb_vmla(tmp, aa[11], bb[13]);
        sb_vmla(tmp, aa[12], bb[12]);
        sb_vmla(tmp, aa[13], bb[11]);
        sb_vmla(tmp, aa[14], bb[10]);
        sb_vmla(tmp, aa[15], bb[9]);
        sb_vstore(&c_mem[32 * 0 + 24], tmp);
        //-----
        sb_vmul(tmp, aa[10], bb[15]);
        sb_vmla(tmp, aa[11], bb[14]);
        sb_vmla(tmp, aa[12], bb[13]);
        sb_vmla(tmp, aa[13], bb[12]);
        sb_vmla(tmp, aa[14], bb[11]);
        sb_vmla(tmp, aa[15], bb[10]);
        sb_vstore(&c_mem[32 * 1 + 24], tmp);
        //-----
        sb_vmul(tmp, aa[11], bb[15]);
        sb_vmla(tmp, aa[12], bb[14]);
        sb_vmla(tmp, aa[13], bb[13]);
        sb_vmla(tmp, aa[14], bb[12]);
        sb_vmla(tmp, aa[15], bb[11]);
        sb_vstore(&c_mem[32 * 2 + 24], tmp);
        //-----
        sb_vmul(tmp, aa[12], bb[15]);
        sb_vmla(tmp, aa[13], bb[14]);
        sb_vmla(tmp, aa[14], bb[13]);
        sb_vmla(tmp, aa[15], bb[12]);
        sb_vstore(&c_mem[32 * 3 + 24], tmp);
        //-----
        sb_vmul(tmp, aa[13], bb[15]);
        sb_vmla(tmp, aa[14], bb[14]);
        sb_vmla(tmp, aa[15], bb[13]);
        sb_vstore(&c_mem[32 * 4 + 24], tmp);
        //-----
        sb_vmul(tmp, aa[14], bb[15]);
        sb_vmla(tmp, aa[15], bb[14]);
        sb_vstore(&c_mem[32 * 5 + 24], tmp);
        //-----
        sb_vmul(tmp, aa[15], bb[15]);
        sb_vstore(&c_mem[32 * 6 + 24], tmp);
        //-----
        sb_vxor(tmp, tmp, tmp);
        sb_vstore(&c_mem[32 * 7 + 24], tmp);

        a_mem += 128;
        b_mem += 128;
        c_mem += 256;
    }
}

void c_schoolbook16x16(uint16_t c[32],
                       const uint16_t b[16],
                       const uint16_t a[16])
{
    uint16x8x2_t bb;
    uint16x8x2_t tmp;
    uint16x8_t aa, zero; 

    bb = vld1q_u16_x2(b);
    zero = vdupq_n_u16(0);

    // 0
    aa = vdupq_n_u16(a[0]);
    sb_vmul(tmp.val[0], bb.val[0], aa);
    sb_vmul(tmp.val[1], bb.val[1], aa);
    sb_vstore(&c[0], tmp.val[0]);
    
    // 1
    tmp.val[0] = vextq_u16(tmp.val[0], tmp.val[1], 1);
    tmp.val[1] = vextq_u16(tmp.val[1], zero, 1);

    aa = vdupq_n_u16(a[1]);
    sb_vmla(tmp.val[0], bb.val[0], aa);
    sb_vmla(tmp.val[1], bb.val[1], aa);
    sb_vstore(&c[1], tmp.val[0]);

    // 2 
    tmp.val[0] = vextq_u16(tmp.val[0], tmp.val[1], 1);
    tmp.val[1] = vextq_u16(tmp.val[1], zero, 1);

    aa = vdupq_n_u16(a[2]);
    sb_vmla(tmp.val[0], bb.val[0], aa);
    sb_vmla(tmp.val[1], bb.val[1], aa);
    sb_vstore(&c[2], tmp.val[0]);

    // 3
    tmp.val[0] = vextq_u16(tmp.val[0], tmp.val[1], 1);
    tmp.val[1] = vextq_u16(tmp.val[1], zero, 1);

    aa = vdupq_n_u16(a[3]);
    sb_vmla(tmp.val[0], bb.val[0], aa);
    sb_vmla(tmp.val[1], bb.val[1], aa);
    sb_vstore(&c[3], tmp.val[0]);

    // 4
    tmp.val[0] = vextq_u16(tmp.val[0], tmp.val[1], 1);
    tmp.val[1] = vextq_u16(tmp.val[1], zero, 1);

    aa = vdupq_n_u16(a[4]);
    sb_vmla(tmp.val[0], bb.val[0], aa);
    sb_vmla(tmp.val[1], bb.val[1], aa);
    sb_vstore(&c[4], tmp.val[0]);

    // 5
    tmp.val[0] = vextq_u16(tmp.val[0], tmp.val[1], 1);
    tmp.val[1] = vextq_u16(tmp.val[1], zero, 1);

    aa = vdupq_n_u16(a[5]);
    sb_vmla(tmp.val[0], bb.val[0], aa);
    sb_vmla(tmp.val[1], bb.val[1], aa);
    sb_vstore(&c[5], tmp.val[0]);

    // 6
    tmp.val[0] = vextq_u16(tmp.val[0], tmp.val[1], 1);
    tmp.val[1] = vextq_u16(tmp.val[1], zero, 1);

    aa = vdupq_n_u16(a[6]);
    sb_vmla(tmp.val[0], bb.val[0], aa);
    sb_vmla(tmp.val[1], bb.val[1], aa);
    sb_vstore(&c[6], tmp.val[0]);

    // 7
    tmp.val[0] = vextq_u16(tmp.val[0], tmp.val[1], 1);
    tmp.val[1] = vextq_u16(tmp.val[1], zero, 1);

    aa = vdupq_n_u16(a[7]);
    sb_vmla(tmp.val[0], bb.val[0], aa);
    sb_vmla(tmp.val[1], bb.val[1], aa);
    sb_vstore(&c[7], tmp.val[0]);

    // 8
    tmp.val[0] = vextq_u16(tmp.val[0], tmp.val[1], 1);
    tmp.val[1] = vextq_u16(tmp.val[1], zero, 1);

    aa = vdupq_n_u16(a[8]);
    sb_vmla(tmp.val[0], bb.val[0], aa);
    sb_vmla(tmp.val[1], bb.val[1], aa);
    sb_vstore(&c[8], tmp.val[0]);

    // 9
    tmp.val[0] = vextq_u16(tmp.val[0], tmp.val[1], 1);
    tmp.val[1] = vextq_u16(tmp.val[1], zero, 1);

    aa = vdupq_n_u16(a[9]);
    sb_vmla(tmp.val[0], bb.val[0], aa);
    sb_vmla(tmp.val[1], bb.val[1], aa);
    sb_vstore(&c[9], tmp.val[0]);

    // 10
    tmp.val[0] = vextq_u16(tmp.val[0], tmp.val[1], 1);
    tmp.val[1] = vextq_u16(tmp.val[1], zero, 1);

    aa = vdupq_n_u16(a[10]);
    sb_vmla(tmp.val[0], bb.val[0], aa);
    sb_vmla(tmp.val[1], bb.val[1], aa);
    sb_vstore(&c[10], tmp.val[0]);

    // 11
    tmp.val[0] = vextq_u16(tmp.val[0], tmp.val[1], 1);
    tmp.val[1] = vextq_u16(tmp.val[1], zero, 1);

    aa = vdupq_n_u16(a[11]);
    sb_vmla(tmp.val[0], bb.val[0], aa);
    sb_vmla(tmp.val[1], bb.val[1], aa);
    sb_vstore(&c[11], tmp.val[0]);

    // 12
    tmp.val[0] = vextq_u16(tmp.val[0], tmp.val[1], 1);
    tmp.val[1] = vextq_u16(tmp.val[1], zero, 1);

    aa = vdupq_n_u16(a[12]);
    sb_vmla(tmp.val[0], bb.val[0], aa);
    sb_vmla(tmp.val[1], bb.val[1], aa);
    sb_vstore(&c[12], tmp.val[0]);

    // 13
    tmp.val[0] = vextq_u16(tmp.val[0], tmp.val[1], 1);
    tmp.val[1] = vextq_u16(tmp.val[1], zero, 1);

    aa = vdupq_n_u16(a[13]);
    sb_vmla(tmp.val[0], bb.val[0], aa);
    sb_vmla(tmp.val[1], bb.val[1], aa);
    sb_vstore(&c[13], tmp.val[0]);

    // 14
    tmp.val[0] = vextq_u16(tmp.val[0], tmp.val[1], 1);
    tmp.val[1] = vextq_u16(tmp.val[1], zero, 1);

    aa = vdupq_n_u16(a[14]);
    sb_vmla(tmp.val[0], bb.val[0], aa);
    sb_vmla(tmp.val[1], bb.val[1], aa);
    sb_vstore(&c[14], tmp.val[0]);

    // 15
    tmp.val[0] = vextq_u16(tmp.val[0], tmp.val[1], 1);
    tmp.val[1] = vextq_u16(tmp.val[1], zero, 1);

    aa = vdupq_n_u16(a[15]);
    sb_vmla(tmp.val[0], bb.val[0], aa);
    sb_vmla(tmp.val[1], bb.val[1], aa);

    vst1q_u16_x2(&c[15], tmp);
    
    c[31] = 0;
}
