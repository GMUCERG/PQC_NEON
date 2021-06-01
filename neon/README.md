# NEON code benchmark result 

All units are clock cycles. 

## Kyber 

- Run `make speed; sudo make bench` to perform benchmark. Require `sudo` to read performance counter in Apple M1. 

| Kyber M1 NEON      | neon Level 1 | neon Level 3 | neon Level 5 |
|--------------------|-------------:|-------------:|-------------:|
| gen_matrix         |        7,680 |       17,944 |       30,743 |
| poly_getnoise_eta1 |        2,698 |        1,289 |        1,289 |
| poly_getnoise_eta2 |        1,299 |        1,289 |        1,287 |
| poly_tomsg         |          295 |          295 |          295 |
| poly_frommsg       |          100 |          100 |          100 |
| neon_ntt           |          413 |          413 |          413 |
| neon_invntt        |          428 |          428 |          428 |
| crypto_kem_keypair |       22,958 |       36,342 |       55,951 |
| crypto_kem_enc     |       32,522 |       49,134 |       71,579 |
| crypto_kem_dec     |       29,412 |       45,100 |       67,026 |
| VectorVectorMul    |        1,858 |        2,545 |        3,272 |
| MatrixVectorMul    |        2,809 |        4,910 |        7,652 |


## NTRU

- Run `make test/speed; sudo test/speed` to perform benchmark. Require `sudo` to read performance counter in Apple M1. 

| NTRU M1 NEON               | neon HPS509 | neon HPS677 | neon HRSS701 | neon HPS821 |
|----------------------------|-------------|-------------|--------------|-------------|
| crypto_kem_keypair         |   2,684,977 |   4,715,457 |    5,032,358 |   6,993,689 |
| crypto_kem_enc             |      39,158 |      60,198 |       23,145 |      75,630 |
| crypto_kem_dec             |      33,024 |      53,708 |       60,644 |      69,213 |
| poly_Rq_mul                |       7,347 |      11,595 |       15,689 |      17,241 |
| poly_S3_mul                |       7,509 |      11,876 |       15,660 |      17,432 |
| randombytes                |          52 |          52 |           52 |          52 |
| randombytes                |          52 |          52 |           52 |          52 |
| sample_iid                 |         162 |         217 |          242 |         279 |
| sample_fixed_type          |      27,853 |      42,254 |        3,425 |      54,036 |
| poly_lift                  |          65 |          89 |        1,074 |         105 |
| poly_Rq_to_S3              |         279 |         383 |          383 |         452 |
| poly_Rq_sum_zero_tobytes   |         449 |         600 |          620 |         159 |
| poly_Rq_sum_zero_frombytes |       1,189 |       1,566 |        1,797 |       1,119 |
| poly_S3_tobytes            |         329 |         435 |          447 |         523 |
| poly_S3_frombytes          |         360 |         501 |          509 |         597 |


## Saber 

- Run `make speed; sudo make bench` to perform benchmark. Require `sudo` to read performance counter in Apple M1. 

| Saber M1 NEON      | neon Level 1 | neon Level 3 | neon Level 5 |
|--------------------|-------------:|-------------:|-------------:|
| GenMatrix          |        8,910 |       20,148 |       34,995 |
| GenSecret          |        4,358 |        4,084 |        4,555 |
| crypto_kem_keypair |       31,277 |       51,352 |       77,037 |
| crypto_kem_enc     |       37,187 |       59,838 |       87,899 |
| crypto_kem_dec     |       35,318 |       57,955 |       86,724 |
| InnerProd          |        3,182 |        4,344 |        5,317 |
| MatrixVectorMul    |        6,641 |       14,028 |       21,608 |


## Saber NTT 

- Run `make all; make verify; sudo make bench` to perform verification and benchmark. Require `sudo` to read performance counter in Apple M1. 

| Saber-NTT M1 NEON  | neon Level 1 | neon Level 3 | neon Level 5 |
|--------------------|-------------:|-------------:|-------------:|
| neon_ntt           |          539 |          539 |          539 |
| neon_invntt        |          519 |          519 |          519 |
| InnerProd          |        6,117 |        8,470 |       10,839 |
| MatrixVectorMul    |       10,084 |       18,931 |       30,393 |