#ifndef PARAMS_H
#define PARAMS_H

#define POLYMUL_SYMBYTES 32
#define POLYMUL_NAMESPACE(s) nttmul##s



#if defined(LIGHTSABER)
//#define POLYMUL_NAMESPACE(s) lightsabermul##s
#define NEGACYCLIC
#define KEM_N 256
#define POLY_N 256
#define NTT_N 256
#define KEM_Q 8192
#define KEM_K 2
#elif defined(SABER)
//#define POLYMUL_NAMESPACE(s) sabermul##s
#define NEGACYCLIC
#define KEM_N 256
#define POLY_N 256
#define NTT_N 256
#define KEM_Q 8192
#define KEM_K 3
#elif defined(FIRESABER)
//#define POLYMUL_NAMESPACE(s) firesabermul##s
#define NEGACYCLIC
#define KEM_N 256
#define POLY_N 256
#define NTT_N 256
#define KEM_Q 8192
#define KEM_K 4
#endif

#endif
