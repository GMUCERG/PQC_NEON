/*=============================================================================
This file has been adapted from the implementation 
(available at, Public Domain https://github.com/KULeuven-COSIC/SABER) 
of "Saber: Module-LWR based key exchange, CPA-secure encryption and CCA-secure KEM"
by : Jan-Pieter D'Anvers, Angshuman Karmakar, Sujoy Sinha Roy, and Frederik Vercauteren
Jose Maria Bermudo Mera, Michiel Van Beirendonck, Andrea Basso. 
=============================================================================*/


#ifndef KEM_H
#define KEM_H
#include <stdint.h>

void indcpa_keypair(uint8_t *pk, uint8_t *sk);

void indcpa_client(uint8_t *pk, uint8_t *b_prime, uint8_t *c, uint8_t *key);

void indcpa_server(uint8_t *pk, uint8_t *b_prime, uint8_t *c, uint8_t *key);

void indcpa_kem_keypair(uint8_t *pk, uint8_t *sk);
void indcpa_kem_enc(uint8_t *message, uint8_t *noiseseed, uint8_t *pk,  uint8_t *ciphertext);
void indcpa_kem_dec(uint8_t *sk, uint8_t *ciphertext, uint8_t message_dec[]);

int crypto_kem_keypair(unsigned char *pk, unsigned char *sk);
int crypto_kem_enc(unsigned char *c, unsigned char *k, const unsigned char *pk);
int crypto_kem_dec(unsigned char *k, const unsigned char *c, const unsigned char *sk);



#endif

