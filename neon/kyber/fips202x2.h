#ifndef FIPS202X2_H
#define FIPS202X2_H

#include <stddef.h>
#include <arm_neon.h>

typedef uint64x2_t v128;

#define FIPS202x2_NAMESPACE(s) pqcrystals_fips202x2_neon##s

#define SHAKE128_RATE 168
#define SHAKE256_RATE 136
#define SHA3_256_RATE 136
#define SHA3_512_RATE 72


typedef struct {
  v128 s[25];
} keccakx2_state;

#define shake128x2_absorb FIPS202x2_NAMESPACE(_shake128x2_absorb)
void shake128x2_absorb(keccakx2_state *state,
                       const uint8_t *in0,
                       const uint8_t *in1,
                       size_t inlen);

#define shake128x2_squeezeblocks FIPS202x2_NAMESPACE(_shake128x2_squeezeblocks)
void shake128x2_squeezeblocks(uint8_t *out0,
                              uint8_t *out1,
                              size_t nblocks,
                              keccakx2_state *state);

#define shake256x2_absorb FIPS202x2_NAMESPACE(_shake256x2_absorb)
void shake256x2_absorb(keccakx2_state *state,
                       const uint8_t *in0,
                       const uint8_t *in1,
                       size_t inlen);

#define shake256x2_squeezeblocks FIPS202x2_NAMESPACE(_shake256x2_squeezeblocks)
void shake256x2_squeezeblocks(uint8_t *out0,
                              uint8_t *out1,
                              size_t nblocks,
                              keccakx2_state *state);

#define shake128x2 FIPS202x2_NAMESPACE(_shake128x2)
void shake128x2(uint8_t *out0,
                uint8_t *out1,
                size_t outlen,
                const uint8_t *in0,
                const uint8_t *in1,
                size_t inlen);

#define shake256x2 FIPS202x2_NAMESPACE(_shake256x2)
void shake256x2(uint8_t *out0,
                uint8_t *out1,
                size_t outlen,
                const uint8_t *in0,
                const uint8_t *in1,
                size_t inlen);

#endif
