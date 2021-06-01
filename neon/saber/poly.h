/*---------------------------------------------------------------------
This file has been adapted from the implementation 
(available at, Public Domain https://github.com/pq-crystals/kyber) 
of "CRYSTALS – Kyber: a CCA-secure module-lattice-based KEM"
by : Joppe Bos, Leo Ducas, Eike Kiltz, Tancrede Lepoint, 
Vadim Lyubashevsky, John M. Schanck, Peter Schwabe & Damien stehle
----------------------------------------------------------------------*/

#ifndef POLY_H
#define POLY_H

#include <stdint.h>
#include "SABER_params.h"

void GenMatrix(uint16_t a[SABER_L][SABER_L][SABER_N], const uint8_t seed[SABER_SEEDBYTES]);
void GenSecret(uint16_t s[SABER_L][SABER_N], const uint8_t seed[SABER_NOISE_SEEDBYTES]);

#endif
