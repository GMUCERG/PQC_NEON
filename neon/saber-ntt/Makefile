# CC ?= /usr/bin/cc
CC = clang
CFLAGS += -Wall -Wextra -Wpedantic -Wmissing-prototypes -Wredundant-decls \
  -Wshadow -Wpointer-arith -fomit-frame-pointer -mtune=native -O3 #-march=native 
RM = /bin/rm
# LIBPAPI = lib/libpapi.a

SOURCES = neon_ntt.c util.c consts256n10753.c consts256n7681.c neon_poly.c neon_polyvec.c 
HEADERS = consts256.h  neon_ntt.h  params.h  util.h  poly.h  polyvec.h

.PHONY: all test speed clean

all: \
	test_ntt_7681 \
	test_ntt_10753 \
	test_polymul512 \
	test_polymul768 \
	test_polymul1024 \
	test_speed512 \
	test_speed768 \
	test_speed1024

test: \
	test_ntt_7681 \
	test_ntt_10753 \
	test_polymul512 \
	test_polymul768 \
	test_polymul1024

speed: \
	test_speed512 \
	test_speed768 \
	test_speed1024

test_ntt_10753: $(SOURCES) $(HEADERS) test_ntt_10753.c 
	$(CC) $(CFLAGS) $(SOURCES) -DSABER test_ntt_10753.c  -o $@

test_ntt_7681: $(SOURCES) $(HEADERS) test_ntt_7681.c 
	$(CC) $(CFLAGS) $(SOURCES) -DSABER test_ntt_7681.c  -o $@

test_polymul512: $(SOURCES) $(HEADERS) test_polymul.c 
	$(CC) $(CFLAGS) $(SOURCES) -DLIGHTSABER test_polymul.c -o $@

test_polymul768: $(SOURCES) $(HEADERS) test_polymul.c 
	$(CC) $(CFLAGS) $(SOURCES) -DSABER test_polymul.c -o $@

test_polymul1024: $(SOURCES) $(HEADERS) test_polymul.c 
	$(CC) $(CFLAGS) $(SOURCES) -DFIRESABER test_polymul.c -o $@

test_speed512: $(SOURCES) $(HEADERS) test_speed.c 
	$(CC) $(CFLAGS) -DLIGHTSABER $(SOURCES) m1cycles.c test_speed.c $(LIBPAPI)  -o $@

test_speed768: $(SOURCES) $(HEADERS) test_speed.c 
	$(CC) $(CFLAGS) -DSABER $(SOURCES) m1cycles.c test_speed.c $(LIBPAPI) -o $@

test_speed1024: $(SOURCES) $(HEADERS) test_speed.c 
	$(CC) $(CFLAGS) -DFIRESABER $(SOURCES) m1cycles.c test_speed.c $(LIBPAPI) -o $@

clean:
	-$(RM) -rf *.gcno *.gcda *.lcov *.o *.so *.req
	-$(RM) -rf test_ntt_7681
	-$(RM) -rf test_ntt_10753
	-$(RM) -rf test_speed512 
	-$(RM) -rf test_speed768 
	-$(RM) -rf test_speed1024 
	-$(RM) -rf test_polymul512
	-$(RM) -rf test_polymul768
	-$(RM) -rf test_polymul1024

bench: 
	./test_speed512
	./test_speed768
	./test_speed1024

verify: 
	./test_polymul512
	./test_polymul768
	./test_polymul1024
