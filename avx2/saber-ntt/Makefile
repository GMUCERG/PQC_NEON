CC ?= cc
CFLAGS += -Wall -Wextra -Wpedantic -Wmissing-prototypes -Wredundant-decls \
  -Wshadow -Wpointer-arith -mavx2 -mbmi2 -mpopcnt -maes \
  -march=native -mtune=native -O3 -fomit-frame-pointer -flto
SABERFLAGS = -march=native -mtune=native -O3 -fomit-frame-pointer
RM = /bin/rm

HEADERS = params.h polyvec.h poly.h cbd.h aes256ctr.h randombytes.h consts256.h consts512.h consts1024.h consts1536.h consts1728.h fq.inc shuffle.inc
SOURCES = poly.c cbd.c aes256ctr.c randombytes.c
SOURCES256N = $(SOURCES) ntt256n.S invntt256n.S basemul256x1.S consts256n7681.c consts256n10753.c
SOURCES512N = $(SOURCES) ntt512n.S invntt512n.S basemul256x2.S consts512.c consts512n7681.c consts512n10753.c
SOURCES1024N = $(SOURCES) ntt1024n.S invntt1024n.S basemul256x4.S consts1024.c consts1024n7681.c consts1024n10753.c
SOURCES1024 = $(SOURCES) ntt1024.S invntt1024.S basemul512x2.S consts1024.c consts10247681.c consts102410753.c
SOURCES1536 = $(SOURCES) ntt1536.S invntt1536.S basemul512x3.S consts15367681.c consts153610753.c
SOURCES1728 = $(SOURCES) ntt1728.S invntt1728.S basemul576x3.S consts17283457.c consts17288641.c
OBJECTS = aes256ctr.o
OBJECTS256NX2 = $(OBJECTS) cbd256.o ntt256n.o invntt256n.o basemul256x1x2.o consts256n7681.o consts256n10753.o
OBJECTS256NX3 = $(OBJECTS) cbd256.o ntt256n.o invntt256n.o basemul256x1x3.o consts256n7681.o consts256n10753.o
OBJECTS256NX4 = $(OBJECTS) cbd256.o ntt256n.o invntt256n.o basemul256x1x4.o consts256n7681.o consts256n10753.o
OBJECTS512N = $(OBJECTS) cbd512.o ntt512n.o invntt512n.o basemul256x2.o consts512.o consts512n7681.o consts512n10753.o
OBJECTS1024N = $(OBJECTS) cbd1024.o ntt1024n.o invntt1024n.o basemul256x4.o consts1024.o consts1024n7681.o consts1024n10753.o
OBJECTS1024 = $(OBJECTS) ntt1024.o invntt1024.o basemul512x2.o consts1024.o consts10247681.o consts102410753.o
OBJECTS1536 = $(OBJECTS) ntt1536.o invntt1536.o basemul512x3.o consts15367681.o consts153610753.o
OBJECTS1728 = $(OBJECTS) ntt1728.o invntt1728.o basemul576x3.o consts17283457.o consts17288641.o

.PHONY: all clean speed

all: \
  test_ntt256n \
  test_basemul256n \
  test_speed256nx2 \
  test_speed256nx3 \
  test_speed256nx4 \
  test_range256n \
  test_ntt512n \
  test_basemul512n \
  test_speed512n \
  test_ntt1024n \
  test_basemul1024n \
  test_speed1024n \
  test_ntt1024 \
  test_basemul1024 \
  test_speed1024 \
  test_range1024 \
  test_ntt1536 \
  test_basemul1536 \
  test_speed1536 \
  test_range1536 \
  test_ntt1728 \
  test_basemul1728 \
  test_speed1728 \
  test_range1728 \
  test_ntruhps509mul \
  test_ntruhps677mul \
  test_ntruhrss701mul \
  test_ntruhps821mul \
  test_lightsabermul \
  test_sabermul \
  test_firesabermul \
  test_lac128mul \
  test_lac192mul

lib: \
  libntruhps509mul.a \
  libntruhps677mul.a \
  libntruhps821mul.a \
  libntruhrss701mul.a \
  liblightsabermul.a \
  libsabermul.a \
  libfiresabermul.a \
  liblac128mul.a \
  liblac192mul.a

speed: \
  test_speed256nx2 \
  test_speed256nx3 \
  test_speed256nx4

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c -o $@ $<

%.o: %.S $(HEADERS)
	$(CC) $(CFLAGS) -c -o $@ $<

cbd256.o: cbd.c $(HEADERS)
	$(CC) $(CFLAGS) -DKEM_N=256 -c -o $@ $<

cbd509.o: cbd.c $(HEADERS)
	$(CC) $(CFLAGS) -DKEM_N=509 -c -o $@ $<

cbd512.o: cbd.c $(HEADERS)
	$(CC) $(CFLAGS) -DKEM_N=512 -c -o $@ $<

cbd677.o: cbd.c $(HEADERS)
	$(CC) $(CFLAGS) -DKEM_N=677 -c -o $@ $<

cbd821.o: cbd.c $(HEADERS)
	$(CC) $(CFLAGS) -DKEM_N=821 -c -o $@ $<

cbd701.o: cbd.c $(HEADERS)
	$(CC) $(CFLAGS) -DKEM_N=701 -c -o $@ $<

cbd1024.o: cbd.c $(HEADERS)
	$(CC) $(CFLAGS) -DKEM_N=1024 -c -o $@ $<

poly_ntruhps509.o: poly.c $(HEADERS)
	$(CC) $(CFLAGS) -DNTRUHPS509 -c -o $@ $<

poly_ntruhps677.o: poly.c $(HEADERS)
	$(CC) $(CFLAGS) -DNTRUHPS677 -c -o $@ $<

poly_ntruhrss701.o: poly.c $(HEADERS)
	$(CC) $(CFLAGS) -DNTRUHRSS701 -c -o $@ $<

poly_ntruhps821.o: poly.c $(HEADERS)
	$(CC) $(CFLAGS) -DNTRUHPS821 -c -o $@ $<

poly_lightsaber.o: poly.c $(HEADERS)
	$(CC) $(CFLAGS) -DLIGHTSABER -c -o $@ $<

poly_saber.o: poly.c $(HEADERS)
	$(CC) $(CFLAGS) -DSABER -c -o $@ $<

poly_firesaber.o: poly.c $(HEADERS)
	$(CC) $(CFLAGS) -DFIRESABER -c -o $@ $<

poly_lac128.o: poly.c $(HEADERS)
	$(CC) $(CFLAGS) -DLAC128 -c -o $@ $<

poly_lac192.o: poly.c $(HEADERS)
	$(CC) $(CFLAGS) -DLAC192 -c -o $@ $<

polyvec_lightsaber.o: polyvec.c $(HEADERS)
	$(CC) $(CFLAGS) -DLIGHTSABER -c -o $@ $<

polyvec_saber.o: polyvec.c $(HEADERS)
	$(CC) $(CFLAGS) -DSABER -c -o $@ $<

polyvec_firesaber.o: polyvec.c $(HEADERS)
	$(CC) $(CFLAGS) -DFIRESABER -c -o $@ $<

sabermul/lightsabermul.o: sabermul/consts.h sabermul/matrix.c sabermul/scm_avx.c sabermul/sabermul.c sabermul/toom-cook_4way.c $(HEADERS)
	$(CC) $(SABERFLAGS) -DLIGHTSABER sabermul/sabermul.c -c -o $@

sabermul/sabermul.o: sabermul/consts.h sabermul/matrix.c sabermul/scm_avx.c sabermul/sabermul.c sabermul/toom-cook_4way.c $(HEADERS)
	$(CC) $(SABERFLAGS) -DSABER sabermul/sabermul.c -c -o $@

sabermul/firesabermul.o: sabermul/consts.h sabermul/matrix.c sabermul/scm_avx.c sabermul/sabermul.c sabermul/toom-cook_4way.c $(HEADERS)
	$(CC) $(SABERFLAGS) -DFIRESABER sabermul/sabermul.c -c -o $@

basemul256x1x2.o: basemul256x1.S $(HEADERS)
	$(CC) -c $(CFLAGS) -DKEM_K=2 -c -o $@ $<

basemul256x1x3.o: basemul256x1.S $(HEADERS)
	$(CC) -c $(CFLAGS) -DKEM_K=3 -c -o $@ $<

basemul256x1x4.o: basemul256x1.S $(HEADERS)
	$(CC) -c $(CFLAGS) -DKEM_K=4 -c -o $@ $<

libntruhps509mul.a: poly_ntruhps509.o $(OBJECTS1024) cbd509.o
	$(AR) -r $@ $^

libntruhps677mul.a: poly_ntruhps677.o $(OBJECTS1536) cbd677.o
	$(AR) -r $@ $^

libntruhrss701mul.a: poly_ntruhrss701.o $(OBJECTS1536) cbd701.o
	$(AR) -r $@ $^

libntruhps821mul.a: poly_ntruhps821.o $(OBJECTS1728) cbd821.o
	$(AR) -r $@ $^

liblightsabermul.a: poly_lightsaber.o polyvec_lightsaber.o $(OBJECTS256NX2)
	$(AR) -r $@ $^

libsabermul.a: poly_saber.o polyvec_saber.o $(OBJECTS256NX3)
	$(AR) -r $@ $^

libfiresabermul.a: poly_firesaber.o polyvec_firesaber.o $(OBJECTS256NX4)
	$(AR) -r $@ $^

liblac128mul.a: poly_lac128.o $(OBJECTS512N)
	$(AR) -r $@ $^

liblac192mul.a: poly_lac192.o $(OBJECTS1024N)
	$(AR) -r $@ $^

test_ntt256n: $(SOURCES256N) $(HEADERS) test_nttn.c cpucycles.c cpucycles.h
	$(CC) $(CFLAGS) $(SOURCES256N) -DLIGHTSABER test_nttn.c cpucycles.c -o $@

test_ntt512n: $(SOURCES512N) $(HEADERS) test_nttn.c cpucycles.c cpucycles.h
	$(CC) $(CFLAGS) $(SOURCES512N) -DLAC128 test_nttn.c cpucycles.c -o $@

test_ntt1024n: $(SOURCES1024N) $(HEADERS) test_nttn.c cpucycles.c cpucycles.h
	$(CC) $(CFLAGS) $(SOURCES1024N) -DLAC192 test_nttn.c cpucycles.c -o $@

test_ntt1024: $(SOURCES1024) $(HEADERS) test_ntt.c cpucycles.c cpucycles.h
	$(CC) $(CFLAGS) $(SOURCES1024) -DNTRUHPS509 test_ntt.c cpucycles.c -o $@

test_ntt1536: $(SOURCES1536) $(HEADERS) test_ntt.c cpucycles.c cpucycles.h
	$(CC) $(CFLAGS) $(SOURCES1536) -DNTRUHPS677 test_ntt.c cpucycles.c -o $@

test_ntt1728: $(SOURCES1728) $(HEADERS) test_ntt.c cpucycles.c cpucycles.h
	$(CC) $(CFLAGS) $(SOURCES1728) -DNTRUHPS821 test_ntt.c cpucycles.c -o $@

test_basemul256n: $(SOURCES256N) $(HEADERS) test_basemuln.c
	$(CC) $(CFLAGS) $(SOURCES256N) -DLIGHTSABER test_basemuln.c -o $@

test_basemul512n: $(SOURCES512N) $(HEADERS) test_basemuln.c
	$(CC) $(CFLAGS) $(SOURCES512N) -DLAC128 test_basemuln.c -o $@

test_basemul1024n: $(SOURCES1024N) $(HEADERS) test_basemuln.c
	$(CC) $(CFLAGS) $(SOURCES1024N) -DLAC192 test_basemuln.c -o $@

test_basemul1024: $(SOURCES1024) $(HEADERS) test_basemul.c
	$(CC) $(CFLAGS) $(SOURCES1024) -DNTRUHPS509 test_basemul.c -o $@

test_basemul1536: $(SOURCES1536) $(HEADERS) test_basemul.c
	$(CC) $(CFLAGS) $(SOURCES1536) -DNTRUHPS677 test_basemul.c -o $@

test_basemul1728: $(SOURCES1728) $(HEADERS) test_basemul.c
	$(CC) $(CFLAGS) $(SOURCES1728) -DNTRUHPS821 test_basemul.c -o $@

test_ntruhps509mul: $(SOURCES1024) $(HEADERS) test_polymul.c ntrumul/ntruhps509_poly_mul.s
	$(CC) $(CFLAGS) $(SOURCES1024) -DNTRUHPS509 test_polymul.c ntrumul/ntruhps509_poly_mul.s -o $@

test_ntruhps677mul: $(SOURCES1536) $(HEADERS) test_polymul.c ntrumul/ntruhps677_poly_mul.s
	$(CC) $(CFLAGS) $(SOURCES1536) -DNTRUHPS677 test_polymul.c ntrumul/ntruhps677_poly_mul.s -o $@

test_ntruhrss701mul: $(SOURCES1536) $(HEADERS) test_polymul.c ntrumul/ntruhrss701_poly_mul.s
	$(CC) $(CFLAGS) $(SOURCES1536) -DNTRUHRSS701 test_polymul.c ntrumul/ntruhrss701_poly_mul.s -o $@

test_ntruhps821mul: $(SOURCES1728) $(HEADERS) test_polymul.c ntrumul/ntruhps821_poly_mul.s
	$(CC) $(CFLAGS) $(SOURCES1728) -DNTRUHPS821 test_polymul.c ntrumul/ntruhps821_poly_mul.s -o $@

test_lightsabermul: $(SOURCES256N) $(HEADERS) test_polyvecmul.c polyvec.c sabermul/lightsabermul.o
	$(CC) $(CFLAGS) $(SOURCES256N) -DLIGHTSABER test_polyvecmul.c polyvec.c sabermul/lightsabermul.o -o $@
test_sabermul: $(SOURCES256N) $(HEADERS) test_polyvecmul.c polyvec.c sabermul/sabermul.o
	$(CC) $(CFLAGS) $(SOURCES256N) -DSABER test_polyvecmul.c polyvec.c sabermul/sabermul.o -o $@

test_firesabermul: $(SOURCES256N) $(HEADERS) test_polyvecmul.c polyvec.c sabermul/firesabermul.o
	$(CC) $(CFLAGS) $(SOURCES256N) -DFIRESABER test_polyvecmul.c polyvec.c sabermul/firesabermul.o -o $@

test_lac128mul: $(SOURCES512N) $(HEADERS) test_polymul.c lacmul/lacmul.c
	$(CC) $(CFLAGS) $(SOURCES512N) -DLAC128 test_polymul.c lacmul/lacmul.c -o $@

test_lac192mul: $(SOURCES1024N) $(HEADERS) test_polymul.c lacmul/lacmul.c
	$(CC) $(CFLAGS) $(SOURCES1024N) -DLAC192 test_polymul.c lacmul/lacmul.c -o $@

test_speed256nx2: $(SOURCES256N) $(HEADERS) test_speed.c polyvec.c sabermul/lightsabermul.o cpucycles.c cpucycles.h speed_print.c speed_print.h
	$(CC) $(CFLAGS) $(SOURCES256N) -DLIGHTSABER test_speed.c polyvec.c sabermul/lightsabermul.o cpucycles.c speed_print.c -o $@

test_speed256nx3: $(SOURCES256N) $(HEADERS) test_speed.c polyvec.c sabermul/sabermul.o cpucycles.c cpucycles.h speed_print.c speed_print.h
	$(CC) $(CFLAGS) $(SOURCES256N) -DSABER test_speed.c polyvec.c sabermul/sabermul.o cpucycles.c speed_print.c -o $@

test_speed256nx4: $(SOURCES256N) $(HEADERS) test_speed.c polyvec.c sabermul/firesabermul.o cpucycles.c cpucycles.h speed_print.c speed_print.h
	$(CC) $(CFLAGS) $(SOURCES256N) -DFIRESABER test_speed.c polyvec.c sabermul/firesabermul.o cpucycles.c speed_print.c -o $@

test_speed512n: $(SOURCES512N) $(HEADERS) test_speed.c lacmul/lacmul.c cpucycles.c cpucycles.h speed_print.c speed_print.h
	$(CC) $(CFLAGS) $(SOURCES512N) -DLAC128 test_speed.c lacmul/lacmul.c cpucycles.c speed_print.c -o $@

test_speed1024n: $(SOURCES1024N) $(HEADERS) test_speed.c lacmul/lacmul.c cpucycles.c cpucycles.h speed_print.c speed_print.h
	$(CC) $(CFLAGS) $(SOURCES1024N) -DLAC192 test_speed.c lacmul/lacmul.c cpucycles.c speed_print.c -o $@

test_speed1024: $(SOURCES1024) $(HEADERS) test_speed.c ntrumul/ntruhps509_poly_mul.s cpucycles.c cpucycles.h speed_print.c speed_print.h
	$(CC) $(CFLAGS) $(SOURCES1024) -DNTRUHPS509 test_speed.c ntrumul/ntruhps509_poly_mul.s cpucycles.c speed_print.c -o $@

test_speed1536: $(SOURCES1536) $(HEADERS) test_speed.c ntrumul/ntruhrss701_poly_mul.s cpucycles.c cpucycles.h speed_print.c speed_print.h
	$(CC) $(CFLAGS) $(SOURCES1536) -DNTRUHRSS701 test_speed.c ntrumul/ntruhrss701_poly_mul.s cpucycles.c speed_print.c -o $@

test_speed1728: $(SOURCES1728) $(HEADERS) test_speed.c ntrumul/ntruhps821_poly_mul.s cpucycles.c cpucycles.h speed_print.c speed_print.h
	$(CC) $(CFLAGS) $(SOURCES1728) -DNTRUHPS821 test_speed.c ntrumul/ntruhps821_poly_mul.s cpucycles.c speed_print.c -o $@

test_range256n: test_range256n.c consts256n7681.c consts256n10753.c consts256.h
	$(CC) $(CFLAGS) -DFIRESABER test_range256n.c consts256n7681.c consts256n10753.c -o $@ -ldb

test_range512n: test_range512n.c consts512n7681.c consts512n10753.c consts512.h
	$(CC) $(CFLAGS) -DLAC128 test_range512n.c consts512n7681.c consts512n10753.c -o $@ -ldb

test_range1024n: test_range1024n.c consts1024n7681.c consts1024n10753.c consts1024.h
	$(CC) $(CFLAGS) -DLAC192 test_range1024n.c consts1024n7681.c consts1024n10753.c -o $@ -ldb

test_range1024: test_range1024.c consts10247681.c consts102410753.c consts1024.h
	$(CC) $(CFLAGS) -DNTRUHPS509 test_range1024.c consts10247681.c consts102410753.c -o $@ -ldb

test_range1536: test_range1536.c consts15367681.c consts153610753.c consts1536.h
	$(CC) $(CFLAGS) -DNTRUHPS677 test_range1536.c consts15367681.c consts153610753.c -o $@ -ldb

test_range1728: test_range1728.c consts17283457.c consts17288641.c consts1728.h
	$(CC) $(CFLAGS) -DNTRUHPS821 test_range1728.c consts17283457.c consts17288641.c -o $@ -ldb

clean:
	-$(RM) -rf *.o sabermul/*.o *.a *.so
	-$(RM) -f access.db
	-$(RM) -f test_ntt256n
	-$(RM) -f test_basemul256n
	-$(RM) -f test_speed256nx2
	-$(RM) -f test_speed256nx3
	-$(RM) -f test_speed256nx4
	-$(RM) -f test_range256n
	-$(RM) -f test_ntt512n
	-$(RM) -f test_basemul512n
	-$(RM) -f test_speed512n
	-$(RM) -f test_range512n
	-$(RM) -f test_ntt1024n
	-$(RM) -f test_basemul1024n
	-$(RM) -f test_speed1024n
	-$(RM) -f test_range1024n
	-$(RM) -f test_ntt1024
	-$(RM) -f test_basemul1024
	-$(RM) -f test_speed1024
	-$(RM) -f test_range1024
	-$(RM) -f test_ntt1536
	-$(RM) -f test_basemul1536
	-$(RM) -f test_speed1536
	-$(RM) -f test_range1536
	-$(RM) -f test_ntt1728
	-$(RM) -f test_basemul1728
	-$(RM) -f test_speed1728
	-$(RM) -f test_range1728
	-$(RM) -f test_ntruhps509mul
	-$(RM) -f test_ntruhps677mul
	-$(RM) -f test_ntruhrss701mul
	-$(RM) -f test_ntruhps821mul
	-$(RM) -f test_lightsabermul
	-$(RM) -f test_sabermul
	-$(RM) -f test_firesabermul
	-$(RM) -f test_lac128mul
	-$(RM) -f test_lac192mul

bench:
	./test_speed256nx2
	./test_speed256nx3
	./test_speed256nx4