CC 		  = /usr/bin/gcc
# CC = clang
CFLAGS  = -Wall -Wextra -Wmissing-prototypes -Wredundant-decls\
	-O3 -fomit-frame-pointer -mtune=native 
NISTFLAGS  = -Wno-unused-result -O3 -fomit-frame-pointer -march=native -std=c99 
CLANG   = clang -march=native -O3 -fomit-frame-pointer -fwrapv -Qunused-arguments
RM 		  = /bin/rm
LDFLAGS="-L/opt/homebrew/opt/openssl@1.1/lib"
CPPFLAGS="-I/opt/homebrew/opt/openssl@1.1/include"

all: test/PQCgenKAT_kem \
     test/test_kex \
     test/kem \

speed: test/speed512 \
	test/speed768 \
	test/speed1024


SOURCES = pack_unpack.c poly.c fips202.c verify.c cbd.c SABER_indcpa.c kem.c
HEADERS = SABER_params.h pack_unpack.h poly.h rng.h fips202.h verify.h cbd.h SABER_indcpa.h

test/test_kex: $(SOURCES) $(HEADERS) rng.o test/test_kex.c
	$(CC) $(CFLAGS) -o $@ $(SOURCES) rng.o test/test_kex.c -lcrypto

test/PQCgenKAT_kem: $(SOURCES) $(HEADERS) rng.o test/PQCgenKAT_kem.c 
	$(CC) $(NISTFLAGS) -o $@ $(SOURCES) rng.o test/PQCgenKAT_kem.c -lcrypto

test/kem: $(SOURCES) $(HEADERS) rng.o test/kem.c
	$(CC) $(CFLAGS) -o $@ $(SOURCES) rng.o test/kem.c -lcrypto

test/speedusec512: $(SOURCES) $(HEADERS) rng.c test/test_speed_usec.c
	$(CC) $(CFLAGS) -DSABER_L=2 -o $@ $(SOURCES) rng.c test/test_speed_usec.c  -lcrypto

test/speedusec768: $(SOURCES) $(HEADERS) rng.c test/test_speed_usec.c
	$(CC) $(CFLAGS) -DSABER_L=3 -o $@ $(SOURCES) rng.c test/test_speed_usec.c  -lcrypto

test/speedusec1024: $(SOURCES) $(HEADERS) rng.c test/test_speed_usec.c
	$(CC) $(CFLAGS) -DSABER_L=4 -o $@ $(SOURCES) rng.c test/test_speed_usec.c  -lcrypto

test/speed512: $(SOURCES) $(HEADERS) rng.c  test/test_speed.c
	$(CC) $(CFLAGS) $(LDFLAGS) $(CPPFLAGS) -DSABER_L=2 -o $@ $(SOURCES) m1cycles.c rng.c  test/test_speed.c  -lcrypto

test/speed768: $(SOURCES) $(HEADERS) rng.c  test/test_speed.c
	$(CC) $(CFLAGS) $(LDFLAGS) $(CPPFLAGS) -DSABER_L=3 -o $@ $(SOURCES) m1cycles.c rng.c  test/test_speed.c  -lcrypto

test/speed1024: $(SOURCES) $(HEADERS) rng.c  test/test_speed.c
	$(CC) $(CFLAGS) $(LDFLAGS) $(CPPFLAGS) -DSABER_L=4 -o $@ $(SOURCES) m1cycles.c rng.c  test/test_speed.c  -lcrypto

rng.o: rng.c
	$(CC) $(NISTFLAGS) $(CPPFLAGS) $(LDFLAGS) -c rng.c -lcrypto -o $@ 

# fips202.o: fips202.c
# 	$(CLANG) -c $^ -o $@

.PHONY: clean test bench

test: 
	./test/test_kex
	./test/PQCgenKAT_kem
	./test/kem

bench:
	./test/speed512
	./test/speed768
	./test/speed1024

clean:
	-$(RM) -f *.o
	-$(RM) -rf test/test_kex
	-$(RM) -rf test/kem
	-$(RM) -rf test/PQCgenKAT_kem
	-$(RM) -f *.req
	-$(RM) -f *.rsp
	-$(RM) -f test/speed512
	-$(RM) -f test/speed768
	-$(RM) -f test/speed1024
	-$(RM) -f test/speedusec512
	-$(RM) -f test/speedusec768
	-$(RM) -f test/speedusec1024