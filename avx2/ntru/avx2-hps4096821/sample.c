#include "sample.h"

void sample_fg(poly *f, poly *g, const unsigned char uniformbytes[NTRU_SAMPLE_FG_BYTES])
{
#ifdef NTRU_HRSS
  sample_iid_plus(f,uniformbytes);
  sample_iid_plus(g,uniformbytes+NTRU_SAMPLE_IID_BYTES);
#endif

#ifdef NTRU_HPS
  sample_iid(f,uniformbytes);
  sample_fixed_type(g,uniformbytes+NTRU_SAMPLE_IID_BYTES);
#endif
}

void sample_rm(poly *r, poly *m, const unsigned char uniformbytes[NTRU_SAMPLE_RM_BYTES])
{
#ifdef NTRU_HRSS
  sample_iid(r,uniformbytes);
  sample_iid(m,uniformbytes+NTRU_SAMPLE_IID_BYTES);
#endif

#ifdef NTRU_HPS
  sample_iid(r,uniformbytes);
  sample_fixed_type(m,uniformbytes+NTRU_SAMPLE_IID_BYTES);
#endif
}

#ifdef NTRU_HRSS
void sample_iid_plus(poly *r, const unsigned char uniformbytes[NTRU_SAMPLE_IID_BYTES])
{
  /* Sample r using sample_iid then conditionally flip    */
  /* signs of even index coefficients so that <x*r, r> >= 0.      */

  int i;
  uint16_t s = 0;

  sample_iid(r, uniformbytes);

  /* Map {0,1,2} -> {0, 1, 2^16 - 1} */
  for(i=0; i<NTRU_N-1; i++)
    r->coeffs[i] = r->coeffs[i] | (-(r->coeffs[i]>>1));

  /* s = <x*r, r>.  (r[n-1] = 0) */
  for(i=0; i<NTRU_N-1; i++)
    s += (uint16_t)((uint32_t)r->coeffs[i + 1] * (uint32_t)r->coeffs[i]);

  /* Extract sign of s (sign(0) = 1) */
  s = 1 | (-(s>>15));

  for(i=0; i<NTRU_N; i+=2)
    r->coeffs[i] = (uint16_t)((uint32_t)s * (uint32_t)r->coeffs[i]);

  /* Map {0,1,2^16-1} -> {0, 1, 2} */
  for(i=0; i<NTRU_N; i++)
    r->coeffs[i] = 3 & (r->coeffs[i] ^ (r->coeffs[i]>>15));
}
#endif

#ifdef NTRU_HPS
void sample_fixed_type(poly *r, const unsigned char u[NTRU_SAMPLE_FT_BYTES])
{
  // Assumes NTRU_SAMPLE_FT_BYTES = ceil(30*(n-1)/8)

  int32_t s[NTRU_N-1];
  int i;

  // Use 30 bits of u per word
  for (i = 0; i < (NTRU_N-1)/4; i++)
  {
    s[4*i+0] =                              (u[15*i+ 0] << 2) + (u[15*i+ 1] << 10) + (u[15*i+ 2] << 18) + ((uint32_t) u[15*i+ 3] << 26);
    s[4*i+1] = ((u[15*i+ 3] & 0xc0) >> 4) + (u[15*i+ 4] << 4) + (u[15*i+ 5] << 12) + (u[15*i+ 6] << 20) + ((uint32_t) u[15*i+ 7] << 28);
    s[4*i+2] = ((u[15*i+ 7] & 0xf0) >> 2) + (u[15*i+ 8] << 6) + (u[15*i+ 9] << 14) + (u[15*i+10] << 22) + ((uint32_t) u[15*i+11] << 30);
    s[4*i+3] =  (u[15*i+11] & 0xfc)       + (u[15*i+12] << 8) + (u[15*i+13] << 16) + ((uint32_t) u[15*i+14] << 24);
  }
#if (NTRU_N - 1) > ((NTRU_N - 1) / 4) * 4 // (N-1) = 2 mod 4
  i = (NTRU_N-1)/4;
  s[4*i+0] =                              (u[15*i+ 0] << 2) + (u[15*i+ 1] << 10) + (u[15*i+ 2] << 18) + ((uint32_t) u[15*i+ 3] << 26);
  s[4*i+1] = ((u[15*i+ 3] & 0xc0) >> 4) + (u[15*i+ 4] << 4) + (u[15*i+ 5] << 12) + (u[15*i+ 6] << 20) + ((uint32_t) u[15*i+ 7] << 28);
#endif

  for (i = 0; i<NTRU_WEIGHT/2; i++) s[i] |=  1;

  for (i = NTRU_WEIGHT/2; i<NTRU_WEIGHT; i++) s[i] |=  2;

  crypto_sort_int32(s,NTRU_N-1);

  for(i=0; i<NTRU_N-1; i++)
    r->coeffs[i] = ((uint16_t) (s[i] & 3));

  r->coeffs[NTRU_N-1] = 0;
}
#endif
