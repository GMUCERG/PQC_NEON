CC 		  = clang
CFLAGS  = -Wall -Wextra -O3 -fomit-frame-pointer -mavx2 -march=native -mtune=native -lcrypto
CFLAGS2  = -Wno-unused-results -O3 -fomit-frame-pointer -mavx2 -march=native -std=c11 
CFLAGS3  = -Wall -Wextra -O3 -fomit-frame-pointer -mavx2 -march=native
CLANG   = clang -march=native -O3 -fomit-frame-pointer -lcrypto -fwrapv -Qunused-arguments -w
RM 		  = /bin/rm


all: test/PQCgenKAT_kem \
     test/test_kex \
     test/test_sample_matrix

speed: \
	test/speed512 \
	test/speed768 \
	test/speed1024

SOURCES = pack_unpack.c verify.c fips202.o SABER_indcpa.c kem.c cbd.c rng.o cpucycles.c
					

SOURCES2 = pack_unpack.c rng.o verify.c fips202.o SABER_indcpa.c kem.c cbd.c cpucycles.c 
					

HEADERS = SABER_params.h pack_unpack.h verify.h cbd.h SABER_indcpa.h kem.h rng.h fips202.h api.h


test/test_kex: $(SOURCES) $(HEADERS) test/test_kex.c 
	$(CC) $(CFLAGS3) -o $@ $(SOURCES) test/test_kex.c -lcrypto

test/test_sample_matrix: $(SOURCES) $(HEADERS) test/sample_matrix_test.c 
	$(CC) $(CFLAGS3) -o $@ $(SOURCES) test/sample_matrix_test.c -lcrypto

test/PQCgenKAT_kem: $(SOURCES2) $(HEADERS) test/PQCgenKAT_kem.c 
	$(CC) $(CFLAGS3) -o $@ $(SOURCES2) test/PQCgenKAT_kem.c -lcrypto  

test/speed512: $(SOURCES2) $(HEADERS) rng.c test/test_speed.c
	$(CC) $(CFLAGS) -DSABER_L=2 -o $@ $(SOURCES2)  test/test_speed.c  -lcrypto

test/speed768: $(SOURCES2) $(HEADERS) rng.c test/test_speed.c
	$(CC) $(CFLAGS) -DSABER_L=3 -o $@ $(SOURCES2)  test/test_speed.c  -lcrypto

test/speed1024: $(SOURCES2) $(HEADERS) rng.c test/test_speed.c
	$(CC) $(CFLAGS) -DSABER_L=4 -o $@ $(SOURCES2) test/test_speed.c  -lcrypto

rng.o: rng.c
	$(CC) $(CFLAGS2) -c rng.c -lcrypto -o $@ 

# fips202.o: fips202.c
# 	$(CLANG) -c $^ -o $@

.PHONY: clean test

test:
	./test/test_kex
	./test/PQCgenKAT_kem


clean:
	-$(RM) -f *.o
	-$(RM) -rf test/test_sample_matrix
	-$(RM) -rf test/test_kex
	-$(RM) -rf test/PQCgenKAT_kem

bench:
	sudo cpupower frequency-set --governor performance
	./test/speed512
	./test/speed768
	./test/speed1024 