#ifndef OWCPA_H
#define OWCPA_H

#include "params.h"
#include "poly.h"

void owcpa_samplemsg(unsigned char msg[NTRU_OWCPA_MSGBYTES],
                     const unsigned char seed[NTRU_SEEDBYTES]);

void owcpa_keypair(unsigned char *pk,
                   unsigned char *sk,
                   const unsigned char seed[NTRU_SEEDBYTES]);

void owcpa_enc(unsigned char *c,
               poly *r,
               const poly *m,
               const unsigned char *pk);

int owcpa_dec(unsigned char *rm,
              const unsigned char *c,
              const unsigned char *sk);
#endif
