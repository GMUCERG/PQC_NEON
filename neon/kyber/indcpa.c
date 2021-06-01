#include <stddef.h>
#include <stdint.h>
#include "params.h"
#include "rejsample.h"
#include "indcpa.h"
#include "poly.h"
#include "polyvec.h"
#include "randombytes.h"
#include "neon_ntt.h"
#include "symmetric.h"

/*************************************************
* Name:        pack_pk
*
* Description: Serialize the public key as concatenation of the
*              serialized vector of polynomials pk
*              and the public seed used to generate the matrix A.
*
* Arguments:   uint8_t *r: pointer to the output serialized public key
*              polyvec *pk: pointer to the input public-key polyvec
*              const uint8_t *seed: pointer to the input public seed
**************************************************/
static void pack_pk(uint8_t r[KYBER_INDCPA_PUBLICKEYBYTES],
                    polyvec *pk,
                    const uint8_t seed[KYBER_SYMBYTES])
{
  size_t i;
  polyvec_tobytes(r, pk);
  for(i=0;i<KYBER_SYMBYTES;i++)
    r[i+KYBER_POLYVECBYTES] = seed[i];
}

/*************************************************
* Name:        unpack_pk
*
* Description: De-serialize public key from a byte array;
*              approximate inverse of pack_pk
*
* Arguments:   - polyvec *pk: pointer to output public-key polynomial vector
*              - uint8_t *seed: pointer to output seed to generate matrix A
*              - const uint8_t *packedpk: pointer to input serialized public key
**************************************************/
static void unpack_pk(polyvec *pk,
                      uint8_t seed[KYBER_SYMBYTES],
                      const uint8_t packedpk[KYBER_INDCPA_PUBLICKEYBYTES])
{
  size_t i;
  polyvec_frombytes(pk, packedpk);
  for(i=0;i<KYBER_SYMBYTES;i++)
    seed[i] = packedpk[i+KYBER_POLYVECBYTES];
}

/*************************************************
* Name:        pack_sk
*
* Description: Serialize the secret key
*
* Arguments:   - uint8_t *r: pointer to output serialized secret key
*              - polyvec *sk: pointer to input vector of polynomials (secret key)
**************************************************/
static void pack_sk(uint8_t r[KYBER_INDCPA_SECRETKEYBYTES], polyvec *sk)
{
  polyvec_tobytes(r, sk);
}

/*************************************************
* Name:        unpack_sk
*
* Description: De-serialize the secret key; inverse of pack_sk
*
* Arguments:   - polyvec *sk: pointer to output vector of polynomials (secret key)
*              - const uint8_t *packedsk: pointer to input serialized secret key
**************************************************/
static void unpack_sk(polyvec *sk, const uint8_t packedsk[KYBER_INDCPA_SECRETKEYBYTES])
{
  polyvec_frombytes(sk, packedsk);
}

/*************************************************
* Name:        pack_ciphertext
*
* Description: Serialize the ciphertext as concatenation of the
*              compressed and serialized vector of polynomials b
*              and the compressed and serialized polynomial v
*
* Arguments:   uint8_t *r: pointer to the output serialized ciphertext
*              poly *pk: pointer to the input vector of polynomials b
*              poly *v: pointer to the input polynomial v
**************************************************/
static void pack_ciphertext(uint8_t r[KYBER_INDCPA_BYTES], polyvec *b, poly *v)
{
  polyvec_compress(r, b);
  poly_compress(r+KYBER_POLYVECCOMPRESSEDBYTES, v);
}

/*************************************************
* Name:        unpack_ciphertext
*
* Description: De-serialize and decompress ciphertext from a byte array;
*              approximate inverse of pack_ciphertext
*
* Arguments:   - polyvec *b: pointer to the output vector of polynomials b
*              - poly *v: pointer to the output polynomial v
*              - const uint8_t *c: pointer to the input serialized ciphertext
**************************************************/
static void unpack_ciphertext(polyvec *b, poly *v, const uint8_t c[KYBER_INDCPA_BYTES])
{
  polyvec_decompress(b, c);
  poly_decompress(v, c+KYBER_POLYVECCOMPRESSEDBYTES);
}


#define gen_a(A,B)  gen_matrix(A,B,0)
#define gen_at(A,B) gen_matrix(A,B,1)

/*************************************************
* Name:        gen_matrix
*
* Description: Deterministically generate matrix A (or the transpose of A)
*              from a seed. Entries of the matrix are polynomials that look
*              uniformly random. Performs rejection sampling on output of
*              a XOF
*
* Arguments:   - polyvec *a: pointer to ouptput matrix A
*              - const uint8_t *seed: pointer to input seed
*              - int transposed: boolean deciding whether A or A^T is generated
**************************************************/
#define GEN_MATRIX_NBLOCKS ((12*KYBER_N/8*(1 << 12)/KYBER_Q + XOF_BLOCKBYTES)/XOF_BLOCKBYTES)
// Not static for benchmarking
void gen_matrix(polyvec *a, const uint8_t seed[KYBER_SYMBYTES], int transposed)
{
  unsigned int ctr0, ctr1, k;
  unsigned int buflen, off;
  uint8_t buf0[GEN_MATRIX_NBLOCKS * XOF_BLOCKBYTES + 2],
      buf1[GEN_MATRIX_NBLOCKS * XOF_BLOCKBYTES + 2];
  neon_xof_state state;

#if KYBER_K == 2
  for (unsigned int i = 0; i < KYBER_K; i++)
  {
    if (transposed)
      neon_xof_absorb(&state, seed, i, i, 0, 1);
    else
      neon_xof_absorb(&state, seed, 0, 1, i, i);

    neon_xof_squeezeblocks(buf0, buf1, GEN_MATRIX_NBLOCKS, &state);

    buflen = GEN_MATRIX_NBLOCKS * XOF_BLOCKBYTES;

    ctr0 = neon_rej_uniform(a[i].vec[0].coeffs, buf0);
    ctr1 = neon_rej_uniform(a[i].vec[1].coeffs, buf1);
    while (ctr0 < KYBER_N || ctr1 < KYBER_N)
    {
      off = buflen % 3;
      for (k = 0; k < off; k++)
      {
        buf0[k] = buf0[buflen - off + k];
        buf1[k] = buf1[buflen - off + k];
      }
      neon_xof_squeezeblocks(buf0 + off, buf1 + off, 1, &state);

      buflen = off + XOF_BLOCKBYTES;
      ctr0 += rej_uniform(a[i].vec[0].coeffs + ctr0, KYBER_N - ctr0, buf0, buflen);
      ctr1 += rej_uniform(a[i].vec[1].coeffs + ctr1, KYBER_N - ctr1, buf1, buflen);
    }
  }
#elif KYBER_K == 3
  int16_t *s1 = NULL, *s2 = NULL;
  unsigned int x1, x2, y1, y2;
  xof_state c_state;

  for (unsigned int j = 0; j < KYBER_K * KYBER_K - 1; j += 2)
  {
    switch (j)
    {
    case 0:
      s1 = a[0].vec[0].coeffs;
      s2 = a[0].vec[1].coeffs;
      x1 = 0;
      y1 = 0;
      x2 = 0;
      y2 = 1;
      break;
    case 2:
      s1 = a[0].vec[2].coeffs;
      s2 = a[1].vec[0].coeffs;
      x1 = 0;
      y1 = 2;
      x2 = 1;
      y2 = 0;
      break;
    case 4:
      s1 = a[1].vec[1].coeffs;
      s2 = a[1].vec[2].coeffs;
      x1 = 1;
      y1 = 1;
      x2 = 1;
      y2 = 2;
      break;
    default:
      s1 = a[2].vec[0].coeffs;
      s2 = a[2].vec[1].coeffs;
      x1 = 2;
      y1 = 0;
      x2 = 2;
      y2 = 1;
      break;
    }

    if (transposed)
      neon_xof_absorb(&state, seed, x1, x2, y1, y2);
    else
      neon_xof_absorb(&state, seed, y1, y2, x1, x2);

    neon_xof_squeezeblocks(buf0, buf1, GEN_MATRIX_NBLOCKS, &state);

    buflen = GEN_MATRIX_NBLOCKS * XOF_BLOCKBYTES;

    ctr0 = neon_rej_uniform(s1, buf0);
    ctr1 = neon_rej_uniform(s2, buf1);

    while (ctr0 < KYBER_N || ctr1 < KYBER_N)
    {
      off = buflen % 3;
      for (k = 0; k < off; k++)
      {
        buf0[k] = buf0[buflen - off + k];
        buf1[k] = buf1[buflen - off + k];
      }
      neon_xof_squeezeblocks(buf0 + off, buf1 + off, 1, &state);

      buflen = off + XOF_BLOCKBYTES;
      ctr0 += rej_uniform(s1 + ctr0, KYBER_N - ctr0, buf0, buflen);
      ctr1 += rej_uniform(s2 + ctr1, KYBER_N - ctr1, buf1, buflen);
    }
  }

  // Last iteration [2][2]
  if (transposed){
    xof_absorb(&c_state, seed, 2, 2);
  }
  else{
    xof_absorb(&c_state, seed, 2, 2);
  }

  xof_squeezeblocks(buf0, GEN_MATRIX_NBLOCKS, &c_state);

  buflen = GEN_MATRIX_NBLOCKS * XOF_BLOCKBYTES;

  ctr0 = neon_rej_uniform(a[2].vec[2].coeffs, buf0);

  while (ctr0 < KYBER_N)
  {
    off = buflen % 3;
    for (k = 0; k < off; k++)
    {
      buf0[k] = buf0[buflen - off + k];
    }
    xof_squeezeblocks(buf0 + off, 1, &c_state);

    buflen = off + XOF_BLOCKBYTES;
    ctr0 += rej_uniform(a[2].vec[2].coeffs + ctr0, KYBER_N - ctr0, buf0, buflen);
  }

#elif KYBER_K == 4
  for (unsigned int i = 0; i < KYBER_K; i++)
  {
    for (unsigned int j = 0; j < KYBER_K; j += 2)
    {
      if (transposed)
        neon_xof_absorb(&state, seed, i, i, j, j + 1);
      else
        neon_xof_absorb(&state, seed, j, j + 1, i, i);

      neon_xof_squeezeblocks(buf0, buf1, GEN_MATRIX_NBLOCKS, &state);
      buflen = GEN_MATRIX_NBLOCKS * XOF_BLOCKBYTES;
      ctr0 = neon_rej_uniform(a[i].vec[j].coeffs, buf0);
      ctr1 = neon_rej_uniform(a[i].vec[j + 1].coeffs, buf1);

      while (ctr0 < KYBER_N || ctr1 < KYBER_N)
      {
        off = buflen % 3;
        for (k = 0; k < off; k++)
        {
          buf0[k] = buf0[buflen - off + k];
          buf1[k] = buf1[buflen - off + k];
        }
        neon_xof_squeezeblocks(buf0 + off, buf1 + off, 1, &state);

        buflen = off + XOF_BLOCKBYTES;
        ctr0 += rej_uniform(a[i].vec[j].coeffs + ctr0, KYBER_N - ctr0, buf0, buflen);
        ctr1 += rej_uniform(a[i].vec[j + 1].coeffs + ctr1, KYBER_N - ctr1, buf1, buflen);
      }
    }
  }
#else
#error "KYBER_K must be in {2,3,4}"
#endif
}

/*************************************************
* Name:        indcpa_keypair
*
* Description: Generates public and private key for the CPA-secure
*              public-key encryption scheme underlying Kyber
*
* Arguments:   - uint8_t *pk: pointer to output public key
*                             (of length KYBER_INDCPA_PUBLICKEYBYTES bytes)
*              - uint8_t *sk: pointer to output private key
                              (of length KYBER_INDCPA_SECRETKEYBYTES bytes)
**************************************************/
void indcpa_keypair(uint8_t pk[KYBER_INDCPA_PUBLICKEYBYTES],
                    uint8_t sk[KYBER_INDCPA_SECRETKEYBYTES])
{
  unsigned int i;
  uint8_t buf[2*KYBER_SYMBYTES];
  const uint8_t *publicseed = buf;
  const uint8_t *noiseseed = buf+KYBER_SYMBYTES;
  polyvec a[KYBER_K], e, pkpv, skpv;

  randombytes(buf, KYBER_SYMBYTES);
  hash_g(buf, buf, KYBER_SYMBYTES);

  gen_a(a, publicseed);

#if KYBER_K == 2
  neon_poly_getnoise_eta1_2x(skpv.vec + 0, skpv.vec + 1, noiseseed, 0, 1);
  neon_poly_getnoise_eta1_2x(e.vec + 0, e.vec + 1, noiseseed, 2, 3);
#elif KYBER_K == 3
  neon_poly_getnoise_eta1_2x(skpv.vec + 0, skpv.vec + 1, noiseseed, 0, 1);
  neon_poly_getnoise_eta1_2x(skpv.vec + 2, e.vec + 0, noiseseed, 2, 3);
  neon_poly_getnoise_eta1_2x(e.vec + 1, e.vec + 2, noiseseed, 4, 5);
#elif KYBER_K == 4
  neon_poly_getnoise_eta1_2x(skpv.vec + 0, skpv.vec + 1, noiseseed, 0, 1);
  neon_poly_getnoise_eta1_2x(skpv.vec + 2, skpv.vec + 3, noiseseed, 2, 3);
  neon_poly_getnoise_eta1_2x(e.vec + 0, e.vec + 1, noiseseed, 4, 5);
  neon_poly_getnoise_eta1_2x(e.vec + 2, e.vec + 3, noiseseed, 6, 7);
#endif

  neon_polyvec_ntt(&skpv);
  neon_polyvec_ntt(&e);

  // matrix-vector multiplication
  for(i=0;i<KYBER_K;i++) {
    neon_polyvec_acc_montgomery(&pkpv.vec[i], &a[i], &skpv, 1);
  }

  neon_polyvec_add_reduce(&pkpv, &e);

  pack_sk(sk, &skpv);
  pack_pk(pk, &pkpv, publicseed);
}

/*************************************************
* Name:        indcpa_enc
*
* Description: Encryption function of the CPA-secure
*              public-key encryption scheme underlying Kyber.
*
* Arguments:   - uint8_t *c:           pointer to output ciphertext
*                                      (of length KYBER_INDCPA_BYTES bytes)
*              - const uint8_t *m:     pointer to input message
*                                      (of length KYBER_INDCPA_MSGBYTES bytes)
*              - const uint8_t *pk:    pointer to input public key
*                                      (of length KYBER_INDCPA_PUBLICKEYBYTES)
*              - const uint8_t *coins: pointer to input random coins
*                                      used as seed (of length KYBER_SYMBYTES)
*                                      to deterministically generate all
*                                      randomness
**************************************************/
void indcpa_enc(uint8_t c[KYBER_INDCPA_BYTES],
                const uint8_t m[KYBER_INDCPA_MSGBYTES],
                const uint8_t pk[KYBER_INDCPA_PUBLICKEYBYTES],
                const uint8_t coins[KYBER_SYMBYTES])
{
  unsigned int i;
  uint8_t seed[KYBER_SYMBYTES];
  polyvec sp, pkpv, ep, at[KYBER_K], b;
  poly v, k, epp;

  unpack_pk(&pkpv, seed, pk);
  poly_frommsg(&k, m);
  gen_at(at, seed);

#if KYBER_K == 2
  // ETA1 != ETA2 (3 != 2)
  neon_poly_getnoise_eta1_2x(sp.vec + 0, sp.vec + 1, coins, 0, 1);
  neon_poly_getnoise_eta2_2x(ep.vec + 0, ep.vec + 1, coins, 2, 3);
  neon_poly_getnoise_eta2(&epp, coins, 4);
#elif KYBER_K == 3
#if KYBER_ETA1 == KYBER_ETA2
  // Because ETA1 == ETA2 
  neon_poly_getnoise_eta1_2x(sp.vec + 0, sp.vec + 1, coins, 0, 1);
  neon_poly_getnoise_eta1_2x(sp.vec + 2, ep.vec + 0, coins, 2, 3);
  neon_poly_getnoise_eta1_2x(ep.vec + 1, ep.vec + 2, coins, 4, 5);
  neon_poly_getnoise_eta2(&epp, coins, 6);
#else
#error "We need eta1 == eta2 here"
#endif
#elif KYBER_K == 4
#if KYBER_ETA1 == KYBER_ETA2
  neon_poly_getnoise_eta1_2x(sp.vec + 0, sp.vec + 1, coins, 0, 1);
  neon_poly_getnoise_eta1_2x(sp.vec + 2, sp.vec + 3, coins, 2, 3);
  neon_poly_getnoise_eta1_2x(ep.vec + 0, ep.vec + 1, coins, 4, 5);
  neon_poly_getnoise_eta1_2x(ep.vec + 2, ep.vec + 3, coins, 6, 7);
  neon_poly_getnoise_eta2(&epp, coins, 8);
#else
#error "We need eta1 == eta2 here"
#endif
#endif

  neon_polyvec_ntt(&sp);

  // matrix-vector multiplication
  for(i=0;i<KYBER_K;i++)
    neon_polyvec_acc_montgomery(&b.vec[i], &at[i], &sp, 0);

  neon_polyvec_acc_montgomery(&v, &pkpv, &sp, 0);

  neon_polyvec_invntt_to_mont(&b);
  neon_invntt(v.coeffs);

  neon_polyvec_add_reduce(&b, &ep);

  neon_poly_add_add_reduce(&v, &epp, &k);

  pack_ciphertext(c, &b, &v);
}

/*************************************************
* Name:        indcpa_dec
*
* Description: Decryption function of the CPA-secure
*              public-key encryption scheme underlying Kyber.
*
* Arguments:   - uint8_t *m:        pointer to output decrypted message
*                                   (of length KYBER_INDCPA_MSGBYTES)
*              - const uint8_t *c:  pointer to input ciphertext
*                                   (of length KYBER_INDCPA_BYTES)
*              - const uint8_t *sk: pointer to input secret key
*                                   (of length KYBER_INDCPA_SECRETKEYBYTES)
**************************************************/
void indcpa_dec(uint8_t m[KYBER_INDCPA_MSGBYTES],
                const uint8_t c[KYBER_INDCPA_BYTES],
                const uint8_t sk[KYBER_INDCPA_SECRETKEYBYTES])
{
  polyvec b, skpv;
  poly v, mp;

  unpack_ciphertext(&b, &v, c);
  unpack_sk(&skpv, sk);

  neon_polyvec_ntt(&b);
  neon_polyvec_acc_montgomery(&mp, &skpv, &b, 0);
  neon_invntt(mp.coeffs);

  neon_poly_sub_reduce(&v, &mp);

  poly_tomsg(m, &v);
}
