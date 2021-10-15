# PQC_NEON
NEON implementation of NIST PQC KEM finalists

## Code

The NEON code used in benchmarking is in `neon/`

## Resources

For active benchmarking on Apple M1 in clock cycles and Cortex-A platform, please see active maintaining projects at:

- Kyber: https://github.com/cothan/kyber/
- Saber: https://github.com/cothan/SABER
- NTRU: https://github.com/cothan/ntru
- Keccak: https://github.com/cothan/NEON-SHA3_2x
- Saber-NTT: The improved NTT will be published in ePrint paper.

## Benchmarking

Compiler option `clang -O3 -mtune=native`:

- We enable maximum optimization on both `ref` and `neon` implementation. We never enforce `-fno-tree-vectorize`.
- `Cortex-A72 @ 1.5 Ghz`, cycles count by [LIBPAPI](https://icl.utk.edu/papi/). In `papi_hl_output` report, we select `PAPI_TOT_CYC` field. 
- `Apple M1 @ 3.2 Ghz`, cycles count inspired by the work of Dougall Johnson.

Unit is **cycles**.


### Apple M1

#### Kyber

| Kyber M1           | ref Level 1 | ref Level 3 | ref Level 5 | neon Level 1 | neon Level 3 | neon Level 5 | ref/neon Level 1 | ref/neon Level 3 | ref/neon Level 5 |
|--------------------|------------:|------------:|------------:|-------------:|-------------:|-------------:|------------------|------------------|------------------|
| ntt                |       3,171 |       3,223 |       3,217 |          240 |          240 |          240 |            13.2  |            13.4  |            13.4  |
| invntt             |       5,171 |       5,118 |       5,148 |          254 |          254 |          254 |            20.4  |            20.1  |            20.3  |
| crypto_kem_keypair |      59,622 |     105,058 |     163,075 |       21,693 |       34,117 |       52,362 |         **2.7**  |         **3.1**  |         **3.1**  |
| crypto_kem_enc     |      76,513 |     120,766 |     175,568 |       30,949 |       46,684 |       68,193 |         **2.5**  |         **2.6**  |         **2.6**  |
| crypto_kem_dec     |      90,254 |     138,813 |     198,509 |       27,052 |       41,574 |       63,476 |         **3.3**  |         **3.3**  |         **3.1**  |
| VectorVectorMul    |      17,549 |      23,401 |      29,388 |        1,105 |        1,512 |        1,922 |            15.9  |            15.5  |            15.3  |
| MatrixVectorMul    |      27,134 |      46,436 |      70,021 |        1,673 |        2,923 |        4,461 |            16.2  |            15.9  |            15.7  |

#### Saber

Note: `ref/neon` = `ref/min(neon, neon-ntt)`, we select best result from Toom-Cook (neon) and NTT (neon-ntt) implementation.

| Saber M1           | ref Level 1 | ref Level 3 | ref Level 5 | neon Level 1 | neon Level 3 | neon Level 5 | neon-ntt Level 1 | neon-ntt Level 3 | neon-ntt Level 5 | ref/neon Level 1 | ref/neon Level 3 | ref/neon Level 5 |
|--------------------|------------:|------------:|------------:|-------------:|-------------:|-------------:|-----------------:|-----------------:|-----------------:|------------------|------------------|------------------|
| crypto_kem_keypair |      44,058 |      74,342 |     119,258 |       31,277 |       51,352 |       77,037 |           29,977 |           47,298 |           71,532 |          **1.5** |          **1.6** |          **1.7** |
| crypto_kem_enc     |      50,850 |      90,407 |     141,025 |       37,187 |       59,838 |       87,899 |           35,949 |           55,892 |           82,776 |          **1.4** |          **1.6** |          **1.7** |
| crypto_kem_dec     |      54,837 |      96,190 |     150,844 |       35,318 |       57,955 |       86,724 |           34,142 |           54,117 |           81,983 |          **1.6** |          **1.8** |          **1.8** |
| InnerProd          |       8,012 |      11,825 |      16,064 |        3,182 |        4,344 |        5,317 |            3,244 |            4,452 |            5,699 |              2.5 |              2.7 |              3.0 |
| MatrixVectorMul    |      16,003 |      35,496 |      64,341 |        6,641 |       14,028 |       21,608 |            5,341 |            9,974 |           16,103 |              3.0 |              3.6 |              4.0 |


##### Saber-NTT

| Apple M1   ||            NEON Encapsulation      |||              NEON Decapsulation      ||
|------------|:---------:|:---------:|:-------------:|:---------:|:---------:|:-------------:|
| 3.2 Ghz    | Toom-Cook | NTT       | Toom-Cook/NTT | Toom-Cook | NTT       | Toom-Cook/NTT |
| lightsaber |    37,187 |   35,949  |          103% |    35,318 |   34,142  |          103% |
| saber      |    59,838 |   55,892  |          107% |    57,955 |   54,117  |          107% |
| firesaber  |    87,899 |   82,776  |          106% |    86,724 |   81,983  |          106% |


#### NTRU

| NTRU M1            | ref HPS509 | ref HPS677 | ref HRSS701 | ref HPS821 | neon HPS509 | neon HPS677 | neon HRSS701 | neon HPS821 | ref/neon HPS509 | ref/neon HPS677 | ref/neon HRSS701 | ref/neon HPS821 |
|--------------------|------------|------------|-------------|------------|-------------|-------------|--------------|-------------|-----------------|-----------------|------------------|-----------------|
| crypto_kem_keypair | 3,501,828  | 6,219,493  | 6,578,665   | 9,056,209  | 2,684,977   | 4,715,457   | 5,032,358    | 6,993,689   |         **1.3** |         **1.3** |          **1.3** |         **1.3** |
| crypto_kem_enc     | 103,059    | 182,986    | 152,390     | 244,308    | 39,158      | 60,198      | 23,145       | 75,630      |         **2.6** |         **3.0** |          **6.6** |         **3.2** |
| crypto_kem_dec     | 231,254    | 429,798    | 439,859     | 583,865    | 33,024      | 53,708      | 60,644       | 69,213      |         **7.0** |         **8.0** |          **7.3** |         **8.4** |
| poly_Rq_mul        | 70,576     | 134,764    | 133,792     | 185,126    | 7,347       | 11,595      | 15,689       | 17,241      |             9.6 |            11.6 |              8.5 |            10.7 |
| poly_S3_mul        | 72,711     | 137,327    | 136,482     | 188,243    | 7,509       | 11,876      | 15,660       | 17,432      |             9.7 |            11.6 |              8.7 |            10.8 |

### Cortex-A72

#### Kyber 

| Kyber A72          | ref Level 1 | ref Level 3 | ref Level 5 | neon Level 1 | neon Level 3 | neon Level 5 | ref/neon Level 1 | ref/neon Level 3 | ref/neon Level 5 |
|--------------------|------------:|------------:|------------:|-------------:|-------------:|-------------:|------------------|------------------|------------------|
| neon_ntt           |       8,496 |       8,500 |       8,551 |        1,162 |        1,162 |        1,162 |              7.3 |              7.3 |              7.4 |
| neon_invntt        |      12,530 |      12,533 |      12,409 |        1,320 |        1,319 |        1,320 |              9.5 |              9.5 |              9.4 |
| crypto_kem_keypair |     136,934 |     237,601 |     371,906 |       68,919 |      110,058 |      177,394 |          **2.0** |          **2.2** |          **2.1** |
| crypto_kem_enc     |     184,533 |     298,928 |     440,645 |       88,152 |      140,100 |      214,150 |          **2.1** |          **2.1** |          **2.1** |
| crypto_kem_dec     |     223,359 |     349,122 |     503,820 |       85,119 |      136,232 |      209,720 |          **2.6** |          **2.6** |          **2.4** |

#### Saber

Note: `ref/neon` = `ref/min(neon, neon-ntt)`, we select best result from Toom-Cook (neon) and NTT (neon-ntt) implementation.


| Saber A72           | ref Level 1 | ref Level 3 | ref Level 5 | neon Level 1 | neon Level 3 | neon Level 5 | neon-ntt Level 1 | neon-ntt Level 3 | neon-ntt Level 5 | ref/neon Level 1 | ref/neon Level 3 | ref/neon Level 5 |
|---------------------|-------------|-------------|-------------|--------------|--------------|--------------|------------------|------------------|------------------|------------------|------------------|------------------|
| crypto_kem_keypair  |     134,149 |     237,166 |     371,778 |      117,500 |      188,545 |      278,417 |          105,616 |          161,717 |          229,870 |          **1.3** |          **1.5** |          **1.6** |
| crypto_kem_enc      |     154,498 |     272,986 |     425,245 |      125,561 |      208,307 |      309,499 |          112,871 |          182,030 |          261,243 |          **1.4** |          **1.5** |          **1.6** |
| crypto_kem_dec      |     166,348 |     294,554 |     463,078 |      125,795 |      211,578 |      321,087 |          112,299 |          185,851 |          273,122 |          **1.5** |          **1.6** |          **1.7** |
| neonInnerProd       |      27,643 |      41,425 |      55,246 |       17,542 |       22,583 |       28,915 |           16,736 |           23,133 |           29,205 |              1.7 |              1.8 |              1.9 |
| neonMatrixVectorMul |      55,991 |     125,811 |     223,951 |       39,804 |       79,128 |      132,226 |           27,920 |           52,300 |           83,679 |              2.0 |              2.4 |              2.7 |

##### Saber-NTT

| NTRU A72   ||            NEON Encapsulation      |||              NEON Decapsulation      ||
|------------|:---------:|:---------:|:-------------:|:---------:|:---------:|:-------------:|
| lightsaber |   125,561 |   112,871 |        111.2% |   125,795 |   112,299 |       112.02% |
| saber      |   208,307 |   182,030 |        114.4% |   211,578 |   185,851 |       113.84% |
| firesaber  |   309,499 |   261,243 |        118.5% |   321,087 |   273,122 |       117.56% |

#### NTRU

| NTRU A72            | ref HPS677 | ref HRSS701 | ref HPS821 | neon HPS677 | neon HRSS701 | neon HPS821 | ref/neon HPS677 | ref/neon HRSS701 | ref/neon HPS821 |
|---------------------|------------|-------------|------------|-------------|--------------|-------------|-----------------|------------------|-----------------|
| crypto_kem_keypair  | 20,925,649 |  24,206,697 | 30,372,301 |  15,898,711 |   19,609,762 |  25,549,949 |         **1.3** |          **1.2** |         **1.2** |
| crypto_kem_enc      |    570,793 |     458,704 |    748,098 |     181,726 |       93,614 |     232,571 |         **3.1** |          **4.9** |         **3.2** |
| crypto_kem_dec      |  1,346,731 |   1,353,368 |  1,829,969 |     205,787 |      262,897 |     274,536 |         **6.5** |          **5.1** |         **6.7** |
| poly_Rq_mul         |    427,673 |     426,803 |    583,892 |      54,971 |       70,061 |      83,500 |             7.8 |              6.1 |             7.0 |
| poly_S3_mul         |    435,218 |     432,773 |    588,746 |      56,094 |       72,230 |      83,099 |             7.8 |              6.0 |             7.1 |


## PQC NEON KEM Finalists Ranking

Notes: _Saber-NTT is selected for comparison._

### Apple M1 ranking

- Unit is in kilocycles (kc).

#### Level 1

| Rank | neon         | Encapsulation    | Speedup | neon         | Decapsulation    | Speedup |
|------|--------------|------------------|---------|--------------|------------------|---------|
|    1 | ntru-hrss701 |             22.8 |    1.00 | Kyber512     |             27.1 |    1.00 |
|    2 | Kyber512     |             30.9 |    1.36 | lightsaber   |             34.1 |    1.26 |
|    3 | lightsaber   |             35.9 |    1.58 | ntru-hps677  |             54.6 |    2.02 |
|    4 | ntru-hps677  |             60.1 |    2.64 | ntru-hrss701 |             60.8 |    2.25 |

#### Level 3

| Rank | neon         | Encapsulation    | Speedup | neon         | Decapsulation    | Speedup |
|------|--------------|------------------|---------|--------------|------------------|---------|
|    1 | Kyber768     |             46.7 |    1.00 | Kyber768     |             41.6 |    1.00 |
|    2 | saber        |             55.9 |    1.20 | saber        |             54.1 |    1.30 |
|    3 | ntru-hps821  |             75.7 |    1.62 | ntru-hps821  |             69.0 |    1.66 |

#### Level 5

| Rank | neon         | Encapsulation    | Speedup | neon         | Decapsulation    | Speedup |
|------|--------------|------------------|---------|--------------|------------------|---------|
|    1 | Kyber1024    |             68.2 |    1.00 | Kyber1024    |             63.5 |    1.00 |
|    2 | firesaber    |             82.8 |    1.21 | firesaber    |             82.0 |    1.29 |

### Cortex-A72 ranking

- Unit is in cycles.

#### Level 1

| Rank | neon         | Encapsulation | Speedup | neon         | Decapsulation | Speedup |
|------|--------------|---------------|---------|--------------|---------------|---------|
| 1    | Kyber512     | 88,152        | 1.00    | Kyber512     | 85,119        | 1.00    |
| 2    | ntru-hrss701 | 93,614        | 1.06    | lightsaber   | 112,299       | 1.32    |
| 3    | lightsaber   | 112,871       | 1.28    | ntru-hps677  | 205,787       | 2.42    |
| 4    | ntru-hps677  | 181,726       | 2.06    | ntru-hrss701 | 262,897       | 3.09    |


#### Level 3 

| Rank | neon        | Encapsulation | Speedup | neon        | Decapsulation | Speedup |
|------|-------------|---------------|---------|-------------|---------------|---------|
|    1 | Kyber768    |       140,100 |    1.00 | Kyber768    |       136,232 |    1.00 |
|    2 | saber       |       182,030 |    1.30 | saber       |       185,851 |    1.36 |
|    3 | ntru-hps821 |       232,571 |    1.66 | ntru-hps821 |       274,536 |    2.02 |


#### Level 5

| Rank | neon      | Encapsulation | Speedup | neon      | Decapsulation | Speedup |
|------|-----------|---------------|---------|-----------|---------------|---------|
|    1 | Kyber1024 |       214,150 |    1.00 | Kyber1024 |       209,720 |    1.00 |
|    2 | firesaber |       261,243 |    1.22 | firesaber |       273,122 |    1.30 |


## Citation

The implementation in this repo is belong to the paper "Fast NEON-Based Multiplication for Lattice-Based NIST Post-quantum Cryptography Finalists" by Duc Tri Nguyen and Prof. Kris Gaj @ CERG GMU. 

```bib
@InProceedings{pqc_neon21,
author="Nguyen, Duc Tri
and Gaj, Kris",
editor="Cheon, Jung Hee
and Tillich, Jean-Pierre",
title="Fast NEON-Based Multiplication for Lattice-Based NIST Post-quantum Cryptography Finalists",
booktitle="Post-Quantum Cryptography",
year="2021",
publisher="Springer International Publishing",
address="Cham",
pages="234--254",
isbn="978-3-030-81293-5"
}

```

## References 

Saber-NTT is faster than Toom-Cook Saber by replacing the old modular reduction with shorter modular reduction (just few instructions!), more details can be found here. 

```bib
@misc{cryptoeprint:2021:986,
    author       = {Hanno Becker and
		    Vincent Hwang and
		    Matthias J. Kannwischer and
		     Bo-Yin Yang and
		    Shang-Yi Yang},
    title        = {Neon NTT: Faster Dilithium, Kyber, and Saber on Cortex-A72 and Apple M1},
    howpublished = {Cryptology ePrint Archive, Report 2021/986},
    year         = {2021},
    note         = {\url{https://ia.cr/2021/986}},
}
```

## License 

Apache 2.0 

Knowledge from this work is taken from public domain, thus, returned to public domain. Feel free to make a pull request.
