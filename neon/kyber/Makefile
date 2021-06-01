#CC ?= /usr/bin/cc -march=native
CC ?= /usr/bin/cc
CFLAGS += -Wall -Wextra -Wpedantic -Wmissing-prototypes -Wredundant-decls \
  -Wshadow -Wpointer-arith -fomit-frame-pointer -mtune=native -O3
NISTFLAGS += -Wno-unused-result -mpopcnt -maes \
   -mtune=native -O3 -fomit-frame-pointer
RM = /bin/rm
# LIBPAPI = lib/libpapi.a

SOURCES = kex.c kem.c indcpa.c polyvec.c neon_polyvec.c poly.c neon_poly.c neon_ntt.c cbd.c reduce.c  verify.c
SOURCESKECCAK = $(SOURCES) fips202x2.c fips202.c neon_symmetric-shake.c symmetric-shake.c rejsample.c
SOURCESNINETIES = $(SOURCES) sha256.c sha512.c aes256ctr.c symmetric-aes.c
HEADERS = params.h kex.h kem.h indcpa.h polyvec.h poly.h neon_ntt.h cbd.h reduce.h verify.h symmetric.h rejsample.h
HEADERSKECCAK = $(HEADERS) fips202.h fips202x2.h
HEADERSNINETIES = $(HEADERS) aes256ctr.h sha2.h

.PHONY: all shared speed clean

all: \
  test_kyber512 \
  test_kyber768 \
  test_kyber1024 \
  test_kex512 \
  test_kex768 \
  test_kex1024 \
  test_vectors512 \
  test_vectors768 \
  test_vectors1024
#   test_kyber512-90s \
#   test_kyber768-90s \
#   test_kyber1024-90s \
#   test_kex512-90s \
#   test_kex768-90s \
#   test_kex1024-90s \
#   test_vectors512-90s \
#   test_vectors768-90s \
#   test_vectors1024-90s

speed: \
  test_speed512_macos \
  test_speed768_macos \
  test_speed1024_macos
#   test_speed512 \
#   test_speed768 \
#   test_speed1024
#   test_speed_ntt
#   test_speed512-90s \
#   test_speed768-90s \
#   test_speed1024-90s

speed_keccak: \
  test_speed512keccak \
  test_speed768keccak \
  test_speed1024keccak

shared: \
  libpqcrystals_kyber512_neon.so \
  libpqcrystals_kyber768_neon.so \
  libpqcrystals_kyber1024_neon.so \
  libpqcrystals_kyber512-90s_neon.so \
  libpqcrystals_kyber768-90s_neon.so \
  libpqcrystals_kyber1024-90s_neon.so \
  libpqcrystals_fips202_neon.so \
  libpqcrystals_sha2_neon.so

libpqcrystals_fips202_neon.so: fips202.c fips202.h
	$(CC) -shared -fPIC $(CFLAGS) fips202.c -o libpqcrystals_fips202_neon.so

libpqcrystals_sha2_neon.so: sha256.c sha512.c sha2.h
	$(CC) -shared -fPIC $(CFLAGS) sha256.c sha512.c -o libpqcrystals_sha2_neon.so

libpqcrystals_kyber512_neon.so: $(SOURCES) $(HEADERS) symmetric-shake.c
	$(CC) -shared -fPIC $(CFLAGS) -DKYBER_K=2 $(SOURCES) symmetric-shake.c -o libpqcrystals_kyber512_neon.so

libpqcrystals_kyber768_neon.so: $(SOURCES) $(HEADERS) symmetric-shake.c
	$(CC) -shared -fPIC $(CFLAGS) -DKYBER_K=3 $(SOURCES) symmetric-shake.c -o libpqcrystals_kyber768_neon.so

libpqcrystals_kyber1024_neon.so: $(SOURCES) $(HEADERS) symmetric-shake.c
	$(CC) -shared -fPIC $(CFLAGS) -DKYBER_K=4 $(SOURCES) symmetric-shake.c -o libpqcrystals_kyber1024_neon.so

test_kyber512: $(SOURCESKECCAK) $(HEADERSKECCAK) test_kyber.c randombytes.c
	$(CC) $(CFLAGS) -DKYBER_K=2 $(SOURCESKECCAK) randombytes.c test_kyber.c -o test_kyber512

test_kyber768: $(SOURCESKECCAK) $(HEADERSKECCAK) test_kyber.c randombytes.c
	$(CC) $(CFLAGS) -DKYBER_K=3 $(SOURCESKECCAK) randombytes.c test_kyber.c -o test_kyber768

test_kyber1024: $(SOURCESKECCAK) $(HEADERSKECCAK) test_kyber.c randombytes.c
	$(CC) $(CFLAGS) -DKYBER_K=4 $(SOURCESKECCAK) randombytes.c test_kyber.c -o test_kyber1024

test_kex512: $(SOURCESKECCAK) $(HEADERSKECCAK) test_kex.c randombytes.c
	$(CC) $(CFLAGS) -DKYBER_K=2 $(SOURCESKECCAK) randombytes.c test_kex.c -o test_kex512

test_kex768: $(SOURCESKECCAK) $(HEADERSKECCAK) test_kex.c randombytes.c
	$(CC) $(CFLAGS) -DKYBER_K=3 $(SOURCESKECCAK) randombytes.c test_kex.c -o test_kex768

test_kex1024: $(SOURCESKECCAK) $(HEADERSKECCAK) test_kex.c randombytes.c
	$(CC) $(CFLAGS) -DKYBER_K=4 $(SOURCESKECCAK) randombytes.c test_kex.c -o test_kex1024

test_vectors512: $(SOURCESKECCAK) $(HEADERSKECCAK) test_vectors.c
	$(CC) $(CFLAGS) -DKYBER_K=2 $(SOURCESKECCAK) test_vectors.c -o test_vectors512

test_vectors768: $(SOURCESKECCAK) $(HEADERSKECCAK) test_vectors.c
	$(CC) $(CFLAGS) -DKYBER_K=3 $(SOURCESKECCAK) test_vectors.c -o test_vectors768

test_vectors1024: $(SOURCESKECCAK) $(HEADERSKECCAK) test_vectors.c
	$(CC) $(CFLAGS) -DKYBER_K=4 $(SOURCESKECCAK) test_vectors.c -o test_vectors1024

test_speed512: $(SOURCESKECCAK) $(HEADERSKECCAK) test_speed_region.c randombytes.c
	$(CC) $(CFLAGS) -DKYBER_K=2 $(SOURCESKECCAK) randombytes.c test_speed_region.c $(LIBPAPI) -o test_speed512

test_speed768: $(SOURCESKECCAK) $(HEADERSKECCAK) test_speed_region.c randombytes.c
	$(CC) $(CFLAGS) -DKYBER_K=3 $(SOURCESKECCAK) randombytes.c test_speed_region.c $(LIBPAPI) -o test_speed768

test_speed1024: $(SOURCESKECCAK) $(HEADERSKECCAK) test_speed_region.c randombytes.c
	$(CC) $(CFLAGS) -DKYBER_K=4 $(SOURCESKECCAK) randombytes.c test_speed_region.c $(LIBPAPI) -o test_speed1024

test_speed512_macos: $(SOURCESKECCAK) $(HEADERSKECCAK) test_speed_region_macos.c randombytes.c m1cycles.c
	$(CC) $(CFLAGS) -DKYBER_K=2 $(SOURCESKECCAK) m1cycles.c randombytes.c test_speed_region_macos.c $(LIBPAPI) -o test_speed512_macos

test_speed768_macos: $(SOURCESKECCAK) $(HEADERSKECCAK) test_speed_region_macos.c randombytes.c m1cycles.c
	$(CC) $(CFLAGS) -DKYBER_K=3 $(SOURCESKECCAK) m1cycles.c randombytes.c test_speed_region_macos.c $(LIBPAPI) -o test_speed768_macos

test_speed1024_macos: $(SOURCESKECCAK) $(HEADERSKECCAK) test_speed_region_macos.c randombytes.c m1cycles.c
	$(CC) $(CFLAGS) -DKYBER_K=4 $(SOURCESKECCAK) m1cycles.c randombytes.c test_speed_region_macos.c $(LIBPAPI) -o test_speed1024_macos


test_speed512keccak: $(SOURCESKECCAK) $(HEADERSKECCAK) speed_print.h speed_print.c test_speed_keccak.c randombytes.c
	$(CC) $(CFLAGS) -DKYBER_K=2 $(SOURCESKECCAK) randombytes.c test_speed_keccak.c $(LIBPAPI) -o test_speed512keccak

test_speed768keccak: $(SOURCESKECCAK) $(HEADERSKECCAK) speed_print.h speed_print.c test_speed_keccak.c randombytes.c
	$(CC) $(CFLAGS) -DKYBER_K=3 $(SOURCESKECCAK) randombytes.c test_speed_keccak.c $(LIBPAPI) -o test_speed768keccak

test_speed1024keccak: $(SOURCESKECCAK) $(HEADERSKECCAK) speed_print.h speed_print.c test_speed_keccak.c randombytes.c
	$(CC) $(CFLAGS) -DKYBER_K=4 $(SOURCESKECCAK) randombytes.c test_speed_keccak.c $(LIBPAPI) -o test_speed1024keccak

test_speed_ntt: neon_ntt.c reduce.c neon_ntt.c speed_ntt.c
	$(CC) $(CFLAGS) reduce.c neon_ntt.c speed_ntt.c $(LIBPAPI) -o test_speed_ntt

# libpqcrystals_kyber512-90s_neon.so: $(SOURCES) $(HEADERS) symmetric-aes.c
# 	$(CC) -shared -fPIC $(CFLAGS) -DKYBER_K=2 -DKYBER_90S $(SOURCES) symmetric-aes.c -o libpqcrystals_kyber512-90s_neon.so

# libpqcrystals_kyber768-90s_neon.so: $(SOURCES) $(HEADERS) symmetric-aes.c
# 	$(CC) -shared -fPIC $(CFLAGS) -DKYBER_K=3 -DKYBER_90S $(SOURCES) symmetric-aes.c -o libpqcrystals_kyber768-90s_neon.so

# libpqcrystals_kyber1024-90s_neon.so: $(SOURCES) $(HEADERS) symmetric-aes.c
# 	$(CC) -shared -fPIC $(CFLAGS) -DKYBER_K=4 -DKYBER_90S $(SOURCES) symmetric-aes.c -o libpqcrystals_kyber1024-90s_neon.so

# test_kyber512-90s: $(SOURCESNINETIES) $(HEADERSNINETIES) test_kyber.c randombytes.c
# 	$(CC) $(CFLAGS) -D KYBER_90S -DKYBER_K=2 $(SOURCESNINETIES) randombytes.c test_kyber.c -o test_kyber512-90s

# test_kyber768-90s: $(SOURCESNINETIES) $(HEADERSNINETIES) test_kyber.c randombytes.c
# 	$(CC) $(CFLAGS) -D KYBER_90S -DKYBER_K=3 $(SOURCESNINETIES) randombytes.c test_kyber.c -o test_kyber768-90s

# test_kyber1024-90s: $(SOURCESNINETIES) $(HEADERSNINETIES) test_kyber.c randombytes.c
# 	$(CC) $(CFLAGS) -D KYBER_90S -DKYBER_K=4 $(SOURCESNINETIES) randombytes.c test_kyber.c -o test_kyber1024-90s

# test_kex512-90s: $(SOURCESNINETIES) $(HEADERSNINETIES) test_kex.c randombytes.c fips202.c fips202.h
# 	$(CC) $(CFLAGS) -D KYBER_90S -DKYBER_K=2 $(SOURCESNINETIES) randombytes.c fips202.c kex.c test_kex.c -o test_kex512-90s

# test_kex768-90s: $(SOURCESNINETIES) $(HEADERSNINETIES) test_kex.c randombytes.c fips202.c fips202.h
# 	$(CC) $(CFLAGS) -D KYBER_90S -DKYBER_K=3 $(SOURCESNINETIES) randombytes.c fips202.c kex.c test_kex.c -o test_kex768-90s

# test_kex1024-90s: $(SOURCESNINETIES) $(HEADERSNINETIES) test_kex.c randombytes.c fips202.c fips202.h
# 	$(CC) $(CFLAGS) -D KYBER_90S -DKYBER_K=4 $(SOURCESNINETIES) randombytes.c fips202.c kex.c test_kex.c -o test_kex1024-90s

# test_vectors512-90s: $(SOURCESNINETIES) $(HEADERSNINETIES) test_vectors.c
# 	$(CC) $(CFLAGS) -D KYBER_90S -DKYBER_K=2 $(SOURCESNINETIES) test_vectors.c -o test_vectors512-90s

# test_vectors768-90s: $(SOURCESNINETIES) $(HEADERSNINETIES) test_vectors.c
# 	$(CC) $(CFLAGS) -D KYBER_90S -DKYBER_K=3 $(SOURCESNINETIES) test_vectors.c -o test_vectors768-90s

# test_vectors1024-90s: $(SOURCESNINETIES) $(HEADERSNINETIES) test_vectors.c
# 	$(CC) $(CFLAGS) -D KYBER_90S -DKYBER_K=4 $(SOURCESNINETIES) test_vectors.c -o test_vectors1024-90s

# test_speed512-90s: $(SOURCESNINETIES) $(HEADERSNINETIES) cpucycles.h cpucycles.c speed_print.h speed_print.c test_speed.c randombytes.c  fips202.c fips202.h
# 	$(CC) $(CFLAGS) -D KYBER_90S -DKYBER_K=2 $(SOURCESNINETIES) randombytes.c cpucycles.c kex.c fips202.c speed_print.c test_speed.c -o test_speed512-90s

# test_speed768-90s: $(SOURCESNINETIES) $(HEADERSNINETIES) cpucycles.h cpucycles.c speed_print.h speed_print.c test_speed.c randombytes.c  fips202.c fips202.h
# 	$(CC) $(CFLAGS) -D KYBER_90S -DKYBER_K=3 $(SOURCESNINETIES) randombytes.c cpucycles.c kex.c fips202.c speed_print.c test_speed.c -o test_speed768-90s

# test_speed1024-90s: $(SOURCESNINETIES) $(HEADERSNINETIES) cpucycles.h cpucycles.c speed_print.h speed_print.c test_speed.c randombytes.c  fips202.c fips202.h
# 	$(CC) $(CFLAGS) -D KYBER_90S -DKYBER_K=4 $(SOURCESNINETIES) randombytes.c cpucycles.c kex.c fips202.c speed_print.c test_speed.c -o test_speed1024-90s

PQCgenKAT_kem512: $(SOURCESKECCAK) $(HEADERSKECCAK) PQCgenKAT_kem.c rng.c rng.h
		$(CC) $(NISTFLAGS) -DKYBER_K=2 -o $@ $(SOURCESKECCAK) rng.c PQCgenKAT_kem.c $(LDFLAGS) -lcrypto

PQCgenKAT_kem768: $(SOURCESKECCAK) $(HEADERSKECCAK) PQCgenKAT_kem.c rng.c rng.h
		$(CC) $(NISTFLAGS) -DKYBER_K=3 -o $@ $(SOURCESKECCAK) rng.c PQCgenKAT_kem.c $(LDFLAGS) -lcrypto

PQCgenKAT_kem1024: $(SOURCESKECCAK) $(HEADERSKECCAK) PQCgenKAT_kem.c rng.c rng.h
		$(CC) $(NISTFLAGS) -DKYBER_K=4 -o $@ $(SOURCESKECCAK) rng.c PQCgenKAT_kem.c $(LDFLAGS) -lcrypto

# PQCgenKAT_kem512-90s: $(SOURCESNINETIES) $(HEADERSNINETIES) PQCgenKAT_kem.c rng.c rng.h
# 		$(CC) $(NISTFLAGS) -DKYBER_K=2 -DKYBER_90S -o $@ $(SOURCESNINETIES) rng.c PQCgenKAT_kem.c $(LDFLAGS) -lcrypto

# PQCgenKAT_kem768-90s: $(SOURCESNINETIES) $(HEADERSNINETIES) PQCgenKAT_kem.c rng.c rng.h
# 		$(CC) $(NISTFLAGS) -DKYBER_K=3 -DKYBER_90S -o $@ $(SOURCESNINETIES) rng.c PQCgenKAT_kem.c $(LDFLAGS) -lcrypto

# PQCgenKAT_kem1024-90s: $(SOURCESNINETIES) $(HEADERSNINETIES) PQCgenKAT_kem.c rng.c rng.h
# 		$(CC) $(NISTFLAGS) -DKYBER_K=4 -DKYBER_90S -o $@ $(SOURCESNINETIES) rng.c PQCgenKAT_kem.c $(LDFLAGS) -lcrypto

clean:
	-$(RM) -rf *.gcno *.gcda *.lcov *.o *.so *.req
	-$(RM) -rf test_kyber512
	-$(RM) -rf test_kyber768
	-$(RM) -rf test_kyber1024
	-$(RM) -rf test_kex512
	-$(RM) -rf test_kex768
	-$(RM) -rf test_kex1024
	-$(RM) -rf test_vectors512
	-$(RM) -rf test_vectors768
	-$(RM) -rf test_vectors1024
	-$(RM) -rf test_speed512
	-$(RM) -rf test_speed768
	-$(RM) -rf test_speed1024
	-$(RM) -rf test_speed512_macos
	-$(RM) -rf test_speed768_macos
	-$(RM) -rf test_speed1024_macos
	-$(RM) -rf test_kyber512-90s
	-$(RM) -rf test_kyber768-90s
	-$(RM) -rf test_kyber1024-90s
	-$(RM) -rf test_kex512-90s
	-$(RM) -rf test_kex768-90s
	-$(RM) -rf test_kex1024-90s
	-$(RM) -rf test_vectors512-90s
	-$(RM) -rf test_vectors768-90s
	-$(RM) -rf test_vectors1024-90s
	-$(RM) -rf test_speed512-90s
	-$(RM) -rf test_speed768-90s
	-$(RM) -rf test_speed1024-90s
	-$(RM) -rf PQCgenKAT_kem512
	-$(RM) -rf PQCgenKAT_kem768
	-$(RM) -rf PQCgenKAT_kem1024
	-$(RM) -rf PQCgenKAT_kem512-90s
	-$(RM) -rf PQCgenKAT_kem768-90s
	-$(RM) -rf PQCgenKAT_kem1024-90s
	-$(RM) -rf test_speedusec512
	-$(RM) -rf test_speedusec768
	-$(RM) -rf test_speedusec1024
	-$(RM) -rf test_speed512keccak
	-$(RM) -rf test_speed768keccak
	-$(RM) -rf test_speed1024keccak
	-$(RM) -rf test_speed_ntt

bench:
	./test_speed512_macos
	./test_speed768_macos
	./test_speed1024_macos
	echo "DONE"

bench_keccak:
	taskset 0x1 ./test_speed512keccak
	taskset 0x1 ./test_speed768keccak
	taskset 0x1 ./test_speed1024keccak
