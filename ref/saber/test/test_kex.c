#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "../api.h"
#include "../poly.h"
#include "../rng.h"
#include "../SABER_indcpa.h"
#include "../verify.h"
#include "../cpucycles.h"

// void fprintBstr(char *S, unsigned char *A, unsigned long long L)
// {
// 	unsigned long long  i;

// 	printf("%s", S);

// 	for ( i=0; i<L; i++ )
// 		printf("%02X", A[i]);

// 	if ( L == 0 )
// 		printf("00");

// 	printf("\n");
// }

uint64_t clock1,clock2;
uint64_t clock_kp_mv,clock_cl_mv, clock_kp_sm, clock_cl_sm;

static int test_kem_cca()
{


  uint8_t pk[SABER_PUBLICKEYBYTES];
  uint8_t sk[SABER_SECRETKEYBYTES];
  uint8_t c[SABER_BYTES_CCA_DEC];	
  uint8_t k_a[SABER_KEYBYTES], k_b[SABER_KEYBYTES];
	
  unsigned char entropy_input[48];
	
  uint64_t i, j, repeat;
  repeat=10000;	
  uint64_t CLOCK1,CLOCK2;
  uint64_t CLOCK_kp,CLOCK_enc,CLOCK_dec;

  	CLOCK1 = 0;
        CLOCK2 = 0;
	CLOCK_kp = CLOCK_enc = CLOCK_dec = 0;
        clock_arith = clock_samp = clock_load = 0;

	time_t t;
   	// Intializes random number generator
   	srand((unsigned) time(&t));

    	for (i=0; i<48; i++){
        	entropy_input[i] = i;
        	//entropy_input[i] = rand()%256;
	}
    	randombytes_init(entropy_input, NULL, 256);


  	for(i=0; i<repeat; i++)
  	{
	    printf("i : %lu\n",i);

	    //Generation of secret key sk and public key pk pair
	    CLOCK1=cpucycles();	
	    crypto_kem_keypair(pk, sk);
	    CLOCK2=cpucycles();	
	    CLOCK_kp=CLOCK_kp+(CLOCK2-CLOCK1);	


	    //Key-Encapsulation call; input: pk; output: ciphertext c, shared-secret k_a;	
	    CLOCK1=cpucycles();
	    crypto_kem_enc(c, k_a, pk);
	    CLOCK2=cpucycles();	
	    CLOCK_enc=CLOCK_enc+(CLOCK2-CLOCK1);	

	
	    //Key-Decapsulation call; input: sk, c; output: shared-secret k_b;	
	    CLOCK1=cpucycles();
	    crypto_kem_dec(k_b, c, sk);
	    CLOCK2=cpucycles();	
	    CLOCK_dec=CLOCK_dec+(CLOCK2-CLOCK1);	
  

	    		
	    // Functional verification: check if k_a == k_b?
	    for(j=0; j<SABER_KEYBYTES; j++)
	    {
		//printf("%u \t %u\n", k_a[j], k_b[j]);
		if(k_a[j] != k_b[j])
		{
			printf("----- ERR CCA KEM ------\n");
			return 0;		
			break;
		}
	    }
   		
  	}
	
	
	printf("Repeat is : %ld\n",repeat);
	printf("Average times key_pair: \t %lu \n",CLOCK_kp/repeat);

	printf("Average times enc: \t %lu \n",CLOCK_enc/repeat);
	printf("Average times dec: \t %lu \n",CLOCK_dec/repeat);
	printf("Average time sample_matrix: \t %lu \n",clock_matrix/repeat);
	printf("Average times sample_secret: \t %lu \n",clock_secret/repeat);
	printf("Average times polynomial mul: \t %lu \n",clock_mul/(3*repeat));
	printf("Average times polynomial mul: \t %lu \n",clock_mul/(3*count_mul));
	printf("Number of times polynomial mul: \t %lu \n",count_mul);


  	return 0;
}
*/
int speed_test_kem_cca()
{

    uint8_t pk[SABER_PUBLICKEYBYTES];
    uint8_t sk[SABER_SECRETKEYBYTES];
    uint8_t c[SABER_BYTES_CCA_DEC];
    uint8_t k_a[SABER_KEYBYTES], k_b[SABER_KEYBYTES];

    int retval;

    unsigned char entropy_input[48];

    uint64_t i, repeat;
    repeat = 10000;
    //repeat = 1;

    time_t t;
    // Intializes random number generator
    srand((unsigned)time(&t));

    for (i = 0; i < 48; i++)
    {
        entropy_input[i] = i;
        //entropy_input[i] = rand()%256;
    }
    randombytes_init(entropy_input, NULL, 256);
    /* =================================== */
    retval = PAPI_hl_region_begin("keypair");
    for (i = 0; i < repeat; i++)
    {
        crypto_kem_keypair(pk, sk);
    }
    retval = PAPI_hl_region_end("keypair");
    if (retval != PAPI_OK)
    {
        return 1;
    }
    /* =================================== */
    retval = PAPI_hl_region_begin("encaps");
    for (i = 0; i < repeat; i++)
    {
        crypto_kem_enc(c, k_a, pk);
    }
    retval = PAPI_hl_region_end("encaps");
    if (retval != PAPI_OK)
    {
        return 1;
    }
    /* =================================== */
    retval = PAPI_hl_region_begin("decaps");
    for (i = 0; i < repeat; i++)
    {
        crypto_kem_dec(k_b, c, sk);
    }
    retval = PAPI_hl_region_end("decaps");
    if (retval != PAPI_OK)
    {
        return 1;
    }
    /* =================================== */
    return 0;
}

int main()
{

	//test_poly_mul();
	//test_poly_sampling();
	//test_sample_matrix();
	// test_kem_cca();
	speed_test_kem_cca();
	return 0;
}
