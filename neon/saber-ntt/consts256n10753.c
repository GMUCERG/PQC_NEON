#include <stdint.h>
#include "params.h"
#include "consts256.h"

#define P 10753
#define MONT 1018 // 2^16 mod p
#define MONT_PINV -6
#define PINV -10751 // p^-1 mod 2^16
#define V 12482 // floor(2^27/p + 0.5)
#define SHIFT 16
#define F 2536 // mont^2/256
#define F_PINV -1560 // PINV*F

__attribute__((aligned(32)))
const int16_t pdata10753[] = {
#define _X8P 0 
    P, P, P, P, P, P, P, P,
#define _X8PINV 8 
    PINV, PINV, PINV, PINV, PINV, PINV, PINV, PINV,
#define _X8MONT 16
    MONT, MONT, MONT, MONT, MONT, MONT, MONT, MONT,
#define _8XF 24
  F, F, F, F, F, F, F, F,
#define _ZETAS 32
    223,  4188, -3688,  2413, -3686,   357,  -376,     0, 
#define _TWIST4 40
    1018,  3688,  -223, -4188,  1018,  3688,  -223, -4188, 
    1018,  4188,   223, -3688,  1018,  4188,   223, -3688, 
    1018,  2413,  4188,   357,  1018,  2413,  4188,   357, 
    1018,   376,  3688,  3686,  1018,   376,  3688,  3686, 
    1018,  2695,  2413,  -425,  1018,  2695,  2413,  -425, 
    1018, -2236,  3686, -3364,  1018, -2236,  3686, -3364, 
    1018,  -425,   357,  -730,  1018,  -425,   357,  -730, 
    1018,  3784,   376, -2236,  1018,  3784,   376, -2236, 
#define _TWIST32_NTT 104
    1018,  -573,  5023, -3535,  1018, -1635,  2045, -2788, 
    1018,  1349,  3615, -5107,  1018,  5313,  5156,  -554, 
    -3091,  1349,  2737, -4889, -3550,  2237,   326,  1927, 
    2982, -2196, -2234,  4328,   193, -5172, -2973, -2788, 
    4875, -5015,  3615,  3891,  4035,  4621,  1356,  4519, 
    2503,  2419,   512,  4967, -4347, -3241,  5341, -2113, 
    -4102,  1992, -1586, -5107,  3062, -2087,  4123,  3360, 
    -2576, -1132, -3169,  1663,  1299,  3410,   326,   624, 
    2695, -5309,   675, -4003,   730,  4447,  -794,  5268, 
    4855,  2050,  4808,  1111, -2236,  4428, -5114, -4973, 
    2982, -2439, -2884,  3419, -4616, -2283,  -400, -1111, 
        4,  2139,  1324, -1689, -2790,  4621,   467,  2807, 
    1931, -2196,  -454, -4540,  3823,  5215,   909, -5083, 
    -2629,    97,   794,  -152,  5175,   274, -2774, -2605, 
    -2388, -2374, -2234,  -834,  2576,  4144, -2684,   825, 
    4742,  3453,  -336,  3125,  3062,  1573,   636, -2279, 
    1018, -3535,  2737, -5015,  1018, -2205, -2206, -5172, 
    1018, -4889, -1586, -2439,  1018,  4403,  -635,  5313, 
    -4102, -5107,   675, -2439,  4379, -1510,  5341,  2237, 
    -2388,  4328,  2981, -4250,  -544, -2205,  5156, -1635, 
    1931, -4540, -2234,  2624,   341,   624,  1268, -2662, 
    -4095,  4720, -3169,  5005,  5213,  -554, -2206,  1930, 
    268,  -825,  2981,  2419, -2790,  4670,  1356,   274, 
    205,  5068,  3441,  2139,   193, -1056,  2045, -5172, 
    -425,  5120,  1572,  2062, -4544, -3995,   636,  4601, 
    3364,  2963,   970, -1722,  3784,  2529, -2973,   778, 
    -2576,  4720,  -567,  2909,  1009,  3360, -2271, -4408, 
    4742,  1204, -5064, -4601,  4379, -2788, -4580,  -458, 
    -5063,  1663, -3715, -5215,  1520,  2813,  -970,  4784, 
    918,  2605, -2740, -1122, -4347, -1510,  -151, -3241, 
    205,   693,  4808,  1409, -4616, -2487,   116,   -40, 
    2790, -2625, -2213, -3410, -3550,  -355,  5341,  3760, 

#define _TWIST32_INVNTT 360
    1018,  4403,  -635,  5313,  1018, -4889, -1586, -2439,
    1018, -2205, -2206, -5172,  1018, -3535,  2737, -5015,
    -544, -2205,  5156, -1635, -2388,  4328,  2981, -4250,
    4379, -1510,  5341,  2237, -4102, -5107,   675, -2439,
    5213,  -554, -2206,  1930, -4095,  4720, -3169,  5005,
    341,   624,  1268, -2662,  1931, -4540, -2234,  2624,
    193, -1056,  2045, -5172,   205,  5068,  3441,  2139,
    -2790,  4670,  1356,   274,   268,  -825,  2981,  2419,
    3784,  2529, -2973,   778,  3364,  2963,   970, -1722,
    -4544, -3995,   636,  4601,  -425,  5120,  1572,  2062,
    4379, -2788, -4580,  -458,  4742,  1204, -5064, -4601,
    1009,  3360, -2271, -4408, -2576,  4720,  -567,  2909,
    -4347, -1510,  -151, -3241,   918,  2605, -2740, -1122,
    1520,  2813,  -970,  4784, -5063,  1663, -3715, -5215,
    -3550,  -355,  5341,  3760,  2790, -2625, -2213, -3410,
    -4616, -2487,   116,   -40,   205,   693,  4808,  1409,
    1018,  5313,  5156,  -554,  1018,  1349,  3615, -5107,
    1018, -1635,  2045, -2788,  1018,  -573,  5023, -3535,
    193, -5172, -2973, -2788,  2982, -2196, -2234,  4328,
    -3550,  2237,   326,  1927, -3091,  1349,  2737, -4889,
    -4347, -3241,  5341, -2113,  2503,  2419,   512,  4967,
    4035,  4621,  1356,  4519,  4875, -5015,  3615,  3891,
    1299,  3410,   326,   624, -2576, -1132, -3169,  1663,
    3062, -2087,  4123,  3360, -4102,  1992, -1586, -5107,
    -2236,  4428, -5114, -4973,  4855,  2050,  4808,  1111,
    730,  4447,  -794,  5268,  2695, -5309,   675, -4003,
    -2790,  4621,   467,  2807,     4,  2139,  1324, -1689,
    -4616, -2283,  -400, -1111,  2982, -2439, -2884,  3419,
    5175,   274, -2774, -2605, -2629,    97,   794,  -152,
    3823,  5215,   909, -5083,  1931, -2196,  -454, -4540,
    3062,  1573,   636, -2279,  4742,  3453,  -336,  3125,
    2576,  4144, -2684,   825, -2388, -2374, -2234,  -834,

};
