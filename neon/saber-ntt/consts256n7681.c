#include <stdint.h>
#include "params.h"
#include "consts256.h"

#define P 7681
#define PINV -7679 // p^-1 mod 2^16
#define MONT -3593 // 2^16 mod p
#define F 1912 // mont^2/256
#define V 17474 // floor(2^27/p + 0.5)
#define SHIFT 16
#define MONT_PINV -9
#define F_PINV -2184 // PINV*F

__attribute__((aligned(32)))
const int16_t pdata7681[] = {
#define _8XP 0
  P, P, P, P, P, P, P, P, 

#define _8XPINV 8
  PINV, PINV, PINV, PINV, PINV, PINV, PINV, PINV,

#define _8XMONT 16
  MONT, MONT, MONT, MONT, MONT, MONT, MONT, MONT,

#define _8XF 24
  F, F, F, F, F, F, F, F,

#define _ZETAS 32
    3777, -3182, 3625, -3696, -1100, 2456, 2194, 0,

#define _TWIST4 40
    -3593,  -3625,  -3777,   3182, -3593,  -3625,  -3777,   3182,
    -3593,  -3182,   3777,   3625, -3593,  -3182,   3777,   3625,
    -3593,  -3696,  -3182,   2456, -3593,  -3696,  -3182,   2456,
    -3593,  -2194,  -3625,   1100, -3593,  -2194,  -3625,   1100,
    -3593,    121,  -3696,  -2319, -3593,    121,  -3696,  -2319,
    -3593,   2495,   1100,   1701, -3593,   2495,   1100,   1701,
    -3593,  -2319,   2456,  -2250, -3593,  -2319,   2456,  -2250,
    -3593,  -1414,  -2194,   2495, -3593,  -1414,  -2194,   2495,

#define _TWIST32_NTT 104
   -3593,    -17,  -1054,   3781,  -3593,   3794,   2732,  -2515,
   -3593,   1712,   2175,  -3343,  -3593,  -3450,  -2883,   1084,
   -3689,   1712,  -1390,  -1689,      7,  -1072,  -1521,   1403,
    -438,  -2378,  -1056,  -3208,   1881,  -3177,   -404,  -2515,
    2816,  -2071,   2175,  -3408,  -1986,  -2001,   3588,  -1931,
   -1599,   2998,   3405,   1441,   2006,    434,      2,  -3752,
    3772,   3434,  -2160,  -3343,    549,  -1779,   -783,   1404,
    -103,   2422,   3750,  -1526,   2956,    226,  -1521,   3745,
     121,   -179,  -3417,   3214,   2250,  -1121,  -1698,  -3312,
     834,   3581,  -3145,  -3677,   2495,  -2891,    730,  -2262,
    -438,   3568,  -1533,  -2874,   3555,  -3461,   2233,   3677,
    -638,   -658,   -486,   -429,   3600,  -2001,  -2133,   -293,
   -1525,  -2378,  -1497,   -642,  -1296,   2059,  -3692,   -796,
     617,  -3770,   1698,   -777,  -3364,  -2918,  -2385,  -3763,
   -1399,  -2247,  -1056,   3657,    103,  -1509,  -1532,    893,
   -2535,  -1242,   1464,  -1837,    549,   -670,  -2764,    589,
   -3593,   3781,  -1390,  -2071,  -3593,  -2083,   2743,  -3177,
   -3593,  -1689,  -2160,   3568,  -3593,   3287,   1168,  -3450,
    3772,  -3343,  -3417,   3568,  -2310,   1519,      2,  -1072,
   -1399,  -3208,  -1756,   2161,   1431,  -2083,  -2883,   3794,
   -1525,   -642,  -1056,   1278,  -1483,   3745,  -2579,   -236,
   -2830,    692,   3750,   2340,  -1921,   1084,   2743,   1407,
     810,   -893,  -1756,   2998,   3600,  -1669,   3588,  -2918,
   -1305,  -2965,    915,   -658,   1881,    402,   2732,  -3177,
   -2319,   3723,   1386,   1203,  -2876,  -2345,  -2764,   -929,
   -1701,  -3335,  -3310,   -222,  -1414,  -2005,   -404,   2719,
    -103,    692,  -3456,   2786,  -1321,   1404,    194,   3550,
   -2535,   3334,   2572,    929,  -2310,  -2515,   -660,   1476,
   -2237,  -1526,   -859,  -2059,   2088,   2258,   3310,    151,
    1993,   3763,  -3428,  -2815,   2006,   1519,  -3816,    434,
   -1305,   1012,  -3145,   1144,   3555,   -592,   2391,   1151,
   -3600,    826,   2789,   -226,      7,    124,      2,   2230,

#define _TWIST32_INVNTT 360
  -3593,  3287,  1168, -3450, -3593, -1689, -2160,  3568, 
  -3593, -2083,  2743, -3177, -3593,  3781, -1390, -2071, 
  1431, -2083, -2883,  3794, -1399, -3208, -1756,  2161, 
  -2310,  1519,     2, -1072,  3772, -3343, -3417,  3568, 
  -1921,  1084,  2743,  1407, -2830,   692,  3750,  2340, 
  -1483,  3745, -2579,  -236, -1525,  -642, -1056,  1278, 
  1881,   402,  2732, -3177, -1305, -2965,   915,  -658, 
  3600, -1669,  3588, -2918,   810,  -893, -1756,  2998, 
  -1414, -2005,  -404,  2719, -1701, -3335, -3310,  -222, 
  -2876, -2345, -2764,  -929, -2319,  3723,  1386,  1203, 
  -2310, -2515,  -660,  1476, -2535,  3334,  2572,   929, 
  -1321,  1404,   194,  3550,  -103,   692, -3456,  2786, 
  2006,  1519, -3816,   434,  1993,  3763, -3428, -2815, 
  2088,  2258,  3310,   151, -2237, -1526,  -859, -2059, 
      7,   124,     2,  2230, -3600,   826,  2789,  -226, 
  3555,  -592,  2391,  1151, -1305,  1012, -3145,  1144, 
  -3593, -3450, -2883,  1084, -3593,  1712,  2175, -3343, 
  -3593,  3794,  2732, -2515, -3593,   -17, -1054,  3781, 
  1881, -3177,  -404, -2515,  -438, -2378, -1056, -3208, 
      7, -1072, -1521,  1403, -3689,  1712, -1390, -1689, 
  2006,   434,     2, -3752, -1599,  2998,  3405,  1441, 
  -1986, -2001,  3588, -1931,  2816, -2071,  2175, -3408, 
  2956,   226, -1521,  3745,  -103,  2422,  3750, -1526, 
    549, -1779,  -783,  1404,  3772,  3434, -2160, -3343, 
  2495, -2891,   730, -2262,   834,  3581, -3145, -3677, 
  2250, -1121, -1698, -3312,   121,  -179, -3417,  3214, 
  3600, -2001, -2133,  -293,  -638,  -658,  -486,  -429, 
  3555, -3461,  2233,  3677,  -438,  3568, -1533, -2874, 
  -3364, -2918, -2385, -3763,   617, -3770,  1698,  -777, 
  -1296,  2059, -3692,  -796, -1525, -2378, -1497,  -642, 
    549,  -670, -2764,   589, -2535, -1242,  1464, -1837, 
    103, -1509, -1532,   893, -1399, -2247, -1056,  3657, 
};