#!/bin/bash 

# Current files
# params.h  neon_poly_mod.c  rq_mul/  neon_sample_iid.c

PRE="../ref-hps2048509"
REF="../ref-hps4096821"
AVX="../avx2-hps4096821"
COM="../ref-common"

ln -s \
$AVX/api_bytes.h \
$REF/api.h \
$REF/crypto_hash_sha3256.h \
$REF/crypto_sort_int32.c \
$REF/crypto_sort_int32.h \
$REF/fips202.c \
$REF/fips202.h \
$REF/kem.c \
$REF/kem.h \
$REF/pack3.c \
$REF/packq.c \
$REF/poly_lift.c \
$REF/poly_r2_inv.c \
$REF/poly_s3_inv.c \
$REF/PQCgenKAT_kem.c \
$REF/randombytes.c \
$REF/randombytes.h \
$REF/sample_iid.c \
$REF/rng.c \
$REF/rng.h \
$REF/sample.c \
$REF/sample.h \
$REF/verify.c \
$REF/verify.h \
$COM/cpucycles.c \
$COM/cpucycles.h . 

cp -r $REF/test .

cp $REF/owcpa.c \
$REF/owcpa.h \
$REF/poly.c \
$REF/poly.h \
$PRE/Makefile \
$PRE/Makefile-NIST \
$AVX/params.h .
