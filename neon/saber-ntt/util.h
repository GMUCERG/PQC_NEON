#ifndef UTIL_H
#define UTIL_H

#include <arm_neon.h>

void print_vector(int16x8x4_t a, const char *string);
void print_array(int16_t *a, int bound, const char *string);
int compare_array(int16_t *a, int16_t *b);

#endif 
