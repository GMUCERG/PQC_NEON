//
//  api.h
//
//  Created by Bassham, Lawrence E (Fed) on 9/6/17.
//  Copyright © 2017 Bassham, Lawrence E (Fed). All rights reserved.
//


//   This is a sample 'api.h' for use 'sign.c'

#ifndef api_h
#define api_h

#if SABER_L == 2
	#define SABER_TYPE 1
	#define SABER_K 2
	#define CRYPTO_ALGNAME "LightSaber"
	#define CRYPTO_SECRETKEYBYTES 1568
	#define CRYPTO_PUBLICKEYBYTES (2*320+32)
	#define CRYPTO_BYTES 32
	#define CRYPTO_CIPHERTEXTBYTES 736
	#define Saber_type 1
#elif SABER_L == 3
	#define SABER_TYPE 2
	#define SABER_K 3
	#define CRYPTO_ALGNAME "Saber"
	#define CRYPTO_SECRETKEYBYTES 2304
	#define CRYPTO_PUBLICKEYBYTES (3*320+32)
	#define CRYPTO_BYTES 32
	#define CRYPTO_CIPHERTEXTBYTES 1088
	#define Saber_type 2
#elif SABER_L == 4
	#define SABER_TYPE 3
	#define SABER_K 4
	#define CRYPTO_ALGNAME "FireSaber"
	#define CRYPTO_SECRETKEYBYTES 3040
	#define CRYPTO_PUBLICKEYBYTES (4*320+32)
	#define CRYPTO_BYTES 32
	#define CRYPTO_CIPHERTEXTBYTES 1472
	#define Saber_type 3
#else
	#error "Unsupported SABER parameter."
#endif

int crypto_kem_keypair(unsigned char *pk, unsigned char *sk);
int crypto_kem_enc(unsigned char *ct, unsigned char *ss, const unsigned char *pk);
int crypto_kem_dec(unsigned char *ss, const unsigned char *ct, const unsigned char *sk);

#endif /* api_h */
