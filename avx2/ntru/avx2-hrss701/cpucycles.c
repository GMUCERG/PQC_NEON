#include "cpucycles.h"

#if __aarch64__

#include <papi.h>

long long cpucycles(void)
{
  //long long result = PAPI_get_real_usec();
  long long result = PAPI_get_real_cyc();
  return result;
}


#else
/*
cpucycles/armv8.c version 20190803
D. J. Bernstein
Public domain.
*/

long long cpucycles(void)
{
  unsigned long long result;
  asm volatile(".byte 15;.byte 49;shlq $32,%%rdx;orq %%rdx,%%rax"
    : "=a" (result) ::  "%rdx");

  return result;
}

#endif
