/*=============================================================================
This file has been adapted from the implementation
(available at, Public Domain https://github.com/KULeuven-COSIC/SABER)
of "Saber: Module-LWR based key exchange, CPA-secure encryption and CCA-secure KEM"
by : Jan-Pieter D'Anvers, Angshuman Karmakar, Sujoy Sinha Roy, and Frederik Vercauteren
Jose Maria Bermudo Mera, Michiel Van Beirendonck, Andrea Basso.

 * Copyright (c) 2020 by Cryptographic Engineering Research Group (CERG)
 * ECE Department, George Mason University
 * Fairfax, VA, U.S.A.
 * Author: Duc Tri Nguyen
=============================================================================*/

#include <arm_neon.h>

#include <string.h>
#include "SABER_indcpa.h"
#include "poly.h"
#include "pack_unpack.h"
#include "neon_poly_rq_mul.h"
#include "rng.h"
#include "fips202.h"
#include "SABER_params.h"

#define h1 (1 << (SABER_EQ - SABER_EP - 1))
#define h2 ((1 << (SABER_EP - 2)) - (1 << (SABER_EP - SABER_ET - 1)) + (1 << (SABER_EQ - SABER_EP - 1)))

void indcpa_kem_keypair(uint8_t pk[SABER_INDCPA_PUBLICKEYBYTES], uint8_t sk[SABER_INDCPA_SECRETKEYBYTES])
{
	uint16_t A[SABER_L][SABER_L][SABER_N];
	uint16_t s[SABER_L][SABER_N];
	uint16_t b[SABER_L][SABER_N] = {0};

	uint8_t seed_A[SABER_SEEDBYTES];
	uint8_t seed_s[SABER_NOISE_SEEDBYTES];

	randombytes(seed_A, SABER_SEEDBYTES);
	shake128(seed_A, SABER_SEEDBYTES, seed_A, SABER_SEEDBYTES); // for not revealing system RNG state
	randombytes(seed_s, SABER_NOISE_SEEDBYTES);

	GenMatrix(A, seed_A);
	GenSecret(s, seed_s);

	// b = tranpose(A*s)
	neonMatrixVectorMul(b, A, s, 1);

	POLVECq2BS(sk, s);
	POLVECp2BS(pk, b);
	memcpy(pk + SABER_POLYVECCOMPRESSEDBYTES, seed_A, sizeof(seed_A));
}

void indcpa_kem_enc(const uint8_t m[SABER_KEYBYTES], const uint8_t seed_sp[SABER_NOISE_SEEDBYTES], const uint8_t pk[SABER_INDCPA_PUBLICKEYBYTES], uint8_t ciphertext[SABER_BYTES_CCA_DEC])
{
	uint16_t A[SABER_L][SABER_L][SABER_N];
	uint16_t sp[SABER_L][SABER_N];
	uint16_t bp[SABER_L][SABER_N] = {0};
	uint16_t vp[SABER_N] = {0};
	uint16_t mp[SABER_N];
	uint16_t b[SABER_L][SABER_N];
	const uint8_t *seed_A = pk + SABER_POLYVECCOMPRESSEDBYTES;

	GenMatrix(A, seed_A);
	GenSecret(sp, seed_sp);

	BS2POLVECp(pk, b);
	BS2POLmsg(m, mp);

	// bp = A*sp
	neonMatrixVectorMul(bp, A, sp, 0);
	// vp = b*sp - mp
	neonInnerProd(vp, b, sp, mp, 1);

	POLVECp2BS(ciphertext, bp);
	POLT2BS(ciphertext + SABER_POLYVECCOMPRESSEDBYTES, vp);
}

void indcpa_kem_dec(const uint8_t sk[SABER_INDCPA_SECRETKEYBYTES], const uint8_t ciphertext[SABER_BYTES_CCA_DEC], uint8_t m[SABER_KEYBYTES])
{

	uint16_t s[SABER_L][SABER_N];
	uint16_t b[SABER_L][SABER_N];
	uint16_t v[SABER_N] = {0};
	uint16_t cm[SABER_N];

	BS2POLVECq(sk, s);
	BS2POLVECp(ciphertext, b);
	BS2POLT(ciphertext + SABER_POLYVECCOMPRESSEDBYTES, cm);

	// v = b*s + h2 - cm
	neonInnerProd(v, b, s, cm, 0);

	POLmsg2BS(m, v);
}
