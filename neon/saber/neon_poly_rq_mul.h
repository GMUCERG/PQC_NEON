/*=============================================================================
 * Copyright (c) 2020 by Cryptographic Engineering Research Group (CERG)
 * ECE Department, George Mason University
 * Fairfax, VA, U.S.A.
 * Author: Duc Tri Nguyen

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

=============================================================================*/
#ifndef NEON_POLY_RQ_MUL_H
#define NEON_POLY_RQ_MUL_H

#include <stdint.h>
#include "SABER_params.h"

void neonInnerProd(uint16_t accumulate[SABER_N],
                   uint16_t polyvecA[SABER_L][SABER_N],
                   uint16_t polyvecB[SABER_L][SABER_N],
                   uint16_t polyC[SABER_N],
                   const unsigned int enc);

void neonMatrixVectorMul(uint16_t vectorB[SABER_L][SABER_N],
                         uint16_t matrixA[SABER_L][SABER_L][SABER_N],
                         uint16_t vectorS[SABER_L][SABER_N], const unsigned int transpose);

#endif
