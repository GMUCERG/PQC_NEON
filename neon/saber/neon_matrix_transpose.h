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
#ifndef NEON_MATRIX_TRANSPOSE_H
#define NEON_MATRIX_TRANSPOSE_H

#include <stdint.h>

#define TRANSPOSE_ITER 8 // Round up of 7*3*3/8
#define TRANSPOSE_ITER_INNER_PROD 6 // Round down of 7*7/8

void transpose_8x32(uint16_t *matrix, const unsigned int iter);
void transpose_8x16(uint16_t *matrix, const unsigned int iter);

#endif
