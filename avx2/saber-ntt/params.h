#ifndef PARAMS_H
#define PARAMS_H

#define POLYMUL_SYMBYTES 32
#define POLYMUL_NAMESPACE(s) nttmul##s

#if defined(NTRUHPS509)
//#define POLYMUL_NAMESPACE(s) ntruhps509mul##s
#define KEM_N 509
#define POLY_N 512
#define NTT_N 1024
#define KEM_Q 2048
#elif defined(NTRUHPS677)
//#define POLYMUL_NAMESPACE(s) ntruhps677mul##s
#define KEM_N 677
#define POLY_N 768
#define NTT_N 1536
#define KEM_Q 2048
#elif defined(NTRUHRSS701)
//#define POLYMUL_NAMESPACE(s) ntruhrss701mul##s
#define KEM_N 701
#define POLY_N 768
#define NTT_N 1536
#define KEM_Q 8192
#elif defined(NTRUHPS821)
//#define POLYMUL_NAMESPACE(s) ntruhps821mul##s
#define KEM_N 821
#define POLY_N 960
#define NTT_N 1728
#define KEM_Q 4096
#elif defined(LAC128)
//#define POLYMUL_NAMESPACE(s) lac128mul##s
#define NEGACYCLIC
#define KEM_N 512
#define POLY_N 512
#define NTT_N 512
#define KEM_Q 256
#elif defined(LAC192)
//#define POLYMUL_NAMESPACE(s) lac192mul##s
#define NEGACYCLIC
#define KEM_N 1024
#define POLY_N 1024
#define NTT_N 1024
#define KEM_Q 256
#elif defined(LIGHTSABER)
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
