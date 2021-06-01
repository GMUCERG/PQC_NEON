#include "../SABER_params.h"
#include "../SABER_indcpa.h"
#include "../kem.h"
#include "../api.h"
#include "../poly.h"
// #include "../randombytes.h"
#include "../rng.h"

#include "../cpucycles.h"
#include "../verify.h"

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <immintrin.h>
#include <string.h>

extern int crypto_kem_keypair(unsigned char *pk, unsigned char *sk);
extern int crypto_kem_enc(unsigned char *ct, unsigned char *ss, const unsigned char *pk);
extern int crypto_kem_dec(unsigned char *ss, const unsigned char *ct, const unsigned char *sk);

int test_kem_cca()
{

	uint8_t pk[SABER_PUBLICKEYBYTES];
	uint8_t sk[SABER_SECRETKEYBYTES];
	uint8_t c[SABER_BYTES_CCA_DEC];
	uint8_t k_a[SABER_KEYBYTES], k_b[SABER_KEYBYTES];

	unsigned char entropy_input[48];

	uint64_t i, j, repeat;
	repeat = 100000;
	//repeat = 1;

	uint64_t CLOCK1, CLOCK2;
	uint64_t CLOCK_kp, CLOCK_enc, CLOCK_dec;
	uint64_t clock_kp_kex, clock_enc_kex, clock_dec_kex;
	uint64_t clock_kp_temp;
	uint64_t clock_mul, clock_matrix, clock_secret, count_mul;

	uint64_t clock_mv_vv_mul;

	uint64_t count_enc;

	CLOCK1 = 0;
	CLOCK2 = 0;
	CLOCK_kp = CLOCK_enc = CLOCK_dec = 0;

	clock_mul = clock_matrix = clock_secret = count_mul = 0;
	clock_kp_kex = clock_enc_kex = clock_dec_kex = 0;
	count_enc = 0;
	clock_mv_vv_mul = 0;

	clock_kp_temp = 0;

	time_t t;
	// Intializes random number generator
	srand((unsigned)time(&t));

	for (i = 0; i < 48; i++)
	{
		entropy_input[i] = i;
		//entropy_input[i] = rand()%256;
	}
	randombytes_init(entropy_input, NULL, 256);

	for (i = 0; i < repeat; i++)
	{
		printf("i : %lu\n", i);

		//Generation of secret key sk and public key pk pair
		CLOCK1 = cpucycles();
		crypto_kem_keypair(pk, sk);
		CLOCK2 = cpucycles();
		CLOCK_kp = CLOCK_kp + (CLOCK2 - CLOCK1);

		//Key-Encapsulation call; input: pk; output: ciphertext c, shared-secret k_a;
		CLOCK1 = cpucycles();
		crypto_kem_enc(c, k_a, pk);
		CLOCK2 = cpucycles();
		CLOCK_enc = CLOCK_enc + (CLOCK2 - CLOCK1);

		//Key-Decapsulation call; input: sk, c; output: shared-secret k_b;
		CLOCK1 = cpucycles();
		crypto_kem_dec(k_b, c, sk);
		CLOCK2 = cpucycles();
		CLOCK_dec = CLOCK_dec + (CLOCK2 - CLOCK1);

		// Functional verification: check if k_a == k_b?
		for (j = 0; j < SABER_KEYBYTES; j++)
		{
			//printf("%u \t %u\n", k_a[j], k_b[j]);
			if (k_a[j] != k_b[j])
			{
				printf("----- ERR CCA KEM ------\n");
				return 0;
				break;
			}
		}
	}

	printf("Repeat is : %ld\n", repeat);
	printf("Average times key_pair: \t %lu \n", CLOCK_kp / repeat);

	printf("Average times enc: \t %lu \n", CLOCK_enc / repeat);
	printf("Average times dec: \t %lu \n", CLOCK_dec / repeat);
	printf("Average time sample_matrix: \t %lu \n", clock_matrix / count_enc);
	printf("Average times sample_secret: \t %lu \n", clock_secret / count_enc);
	printf("Average times (matrix_vector + vector_vector) mul: \t %lu \n", clock_mv_vv_mul / count_enc);
	printf("Average times single (matrix_vector + vector_vector) mul: \t %lu \n", clock_mv_vv_mul / ((SABER_K * SABER_K + SABER_K) * count_enc));
	printf("Average times polynomial mul: \t %lu \n", clock_mul / (SABER_K * count_mul));
	printf("Only indcpa key-pair: \t %lu \n", clock_kp_kex / repeat);
	printf("Time for (KEM_kp-indcpa_kp): \t %lu \n", clock_kp_temp / repeat);
	printf("Only indcpa enc: \t %lu \n", clock_enc_kex / repeat);
	printf("Number of times polynomial mul: \t %lu \n", count_mul);

	return 0;
}

#define NTESTS 1000000

#define TIME(s) clock_gettime(CLOCK_MONOTONIC_RAW, &s);
// Result is nanosecond per call
#define CALC(start, stop) \
	((double)((stop.tv_sec - start.tv_sec) * 1000000000 + (stop.tv_nsec - start.tv_nsec))) / NTESTS;

int test_functions()
{

	uint8_t seed[SABER_SEEDBYTES] = {0};
	unsigned int i;
	unsigned char pk[CRYPTO_PUBLICKEYBYTES] = {0};
	unsigned char sk[CRYPTO_SECRETKEYBYTES] = {0};
	unsigned char ct[CRYPTO_CIPHERTEXTBYTES] = {0};
	unsigned char key[CRYPTO_BYTES] = {0};

	uint16_t matrix[SABER_K][SABER_K][SABER_N] = {0};
	uint16_t s[SABER_K][SABER_N] = {0};
	uint16_t a[SABER_K][SABER_N] = {0};
	uint16_t b[SABER_K][SABER_N] = {0};
	uint16_t acc[SABER_N] = {0};
	struct timespec start, stop;
	long ns;

	TIME(start);
	for (i = 0; i < NTESTS; i++)
	{
		GenMatrix(matrix, seed);
	}
	TIME(stop);
	ns = CALC(start, stop);
	printf("GenMatrix: %lld\n", ns);

	TIME(start);
	for (i = 0; i < NTESTS; i++)
	{
		GenSecret(s, seed);
	}
	TIME(stop);
	ns = CALC(start, stop);
	printf("GenSecret: %lld\n", ns);

	TIME(start);
	for (i = 0; i < NTESTS; i++)
	{
		crypto_kem_keypair(pk, sk);
	}
	TIME(stop);
	ns = CALC(start, stop);
	printf("crypto_kem_keypair: %lld\n", ns);

	TIME(start);
	for (i = 0; i < NTESTS; i++)
	{
		crypto_kem_enc(ct, key, pk);
	}
	TIME(stop);
	ns = CALC(start, stop);
	printf("crypto_kem_enc: %lld\n", ns);

	TIME(start);
	for (i = 0; i < NTESTS; i++)
	{
		crypto_kem_dec(key, ct, sk);
	}
	TIME(stop);
	ns = CALC(start, stop);
	printf("crypto_kem_dec: %lld\n", ns);

	// TIME(start);
	// for(i=0;i<NTESTS;i++) {
	//   InnerProd(a, b, acc);
	// }
	// TIME(stop);
	// ns = CALC(start, stop);
	// printf("InnerProd: %lld\n", ns);

	// TIME(start);
	// for(i=0;i<NTESTS;i++) {
	//   MatrixVectorMul(matrix, s, b, 1);
	// }
	// TIME(stop);
	// ns = CALC(start, stop);
	// printf("MatrixVectorMul: %lld\n", ns);
}

int main()
{

	//test_poly_mul();
	//test_poly_sampling();
	//test_sample_matrix();
	// test_kem_cca();
	test_functions();
	return 0;
}
