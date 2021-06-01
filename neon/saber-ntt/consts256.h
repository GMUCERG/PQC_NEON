#ifndef CONSTS256_H
#define CONSTS256_H

#include "params.h"

/* qdata */
#define _8XP                0
#define _8XPINV             8
#define _8XMONT            16
#define _8XF               24
#define _ZETAS             32
#define _TWIST4            40
#define _TWIST32_NTT      104
#define _TWIST32_INVNTT   360
// #define _8XV            32
// #define _8XSHIFT        48
// #define _8XMONT_PINV    64
// #define _8XF_PINV       96
// #define _8XF           112

/* idx */
#define _REVIDXW         0
#define _REVIDXD        32
#define _SIGNMSKW       64

#include <stdint.h>

#define idxdata POLYMUL_NAMESPACE(_idxdata)
extern const uint8_t idxdata[];
#define pdata7681 POLYMUL_NAMESPACE(_pdata7681)
extern const int16_t pdata7681[];
#define pdata10753 POLYMUL_NAMESPACE(_pdata10753)
extern const int16_t pdata10753[];

#define P7681 7681
#define P10753 10753
#define CRT_U_PINV 32747
#define CRT_U 3563
#define PDATA7681 pdata7681
#define PDATA10753 pdata10753

#endif
