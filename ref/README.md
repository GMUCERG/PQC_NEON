# Reference code benchmark result 

All units are clock cycles. 

## Kyber 

- Run `make speed; sudo make bench` to perform benchmark. Require `sudo` to read performance counter in Apple M1. 

| Kyber M1 REF       | ref Level 1 | ref Level 3 | ref Level 5 |
|--------------------|------------:|------------:|------------:|
| gen_matrix         |      11,989 |      26,894 |      47,558 |
| poly_getnoise_eta1 |       1,939 |       1,458 |       1,459 |
| poly_getnoise_eta2 |       1,458 |       1,458 |       1,458 |
| poly_tomsg         |         295 |         296 |         295 |
| poly_frommsg       |         100 |         100 |         100 |
| ref_ntt            |       3,171 |       3,223 |       3,217 |
| ref_invntt         |       5,171 |       5,118 |       5,148 |
| crypto_kem_keypair |      59,622 |     105,058 |     163,075 |
| crypto_kem_enc     |      76,513 |     120,766 |     175,568 |
| crypto_kem_dec     |      90,254 |     138,813 |     198,509 |
| VectorVectorMul    |      17,549 |      23,401 |      29,388 |
| MatrixVectorMul    |      27,134 |      46,436 |      70,021 |


## NTRU

- Run `make test/speed; sudo test/speed` to perform benchmark. Require `sudo` to read performance counter in Apple M1. 


| NTRU M1 REF                | ref HPS509 | ref HPS677 | ref HRSS701 | ref HPS821 |
|----------------------------|------------|------------|-------------|------------|
| crypto_kem_keypair         |  3,501,828 |  6,219,493 |   6,578,665 |  9,056,209 |
| crypto_kem_enc             |    103,059 |    182,986 |     152,390 |    244,308 |
| crypto_kem_dec             |    231,254 |    429,798 |     439,859 |    583,865 |
| poly_Rq_mul                |     70,576 |    134,764 |     133,792 |    185,126 |
| poly_S3_mul                |     72,711 |    137,327 |     136,482 |    188,243 |
| randombytes                |         52 |         52 |          52 |         52 |
| randombytes                |         52 |         52 |          52 |         52 |
| sample_iid                 |        165 |        234 |         225 |        279 |
| sample_fixed_type          |     27,808 |     42,299 |       3,422 |     53,976 |
| poly_lift                  |        115 |        176 |      12,193 |        165 |
| poly_Rq_to_S3              |      2,209 |      2,836 |       2,936 |      3,504 |
| poly_Rq_sum_zero_tobytes   |        449 |        600 |         620 |        159 |
| poly_Rq_sum_zero_frombytes |      1,189 |      1,567 |       1,797 |      1,119 |
| poly_S3_tobytes            |        328 |        433 |         447 |        523 |
| poly_S3_frombytes          |      2,299 |      2,978 |       3,086 |      3,653 |


## Saber 

- Run `make speed; sudo make bench` to perform benchmark. Require `sudo` to read performance counter in Apple M1. 

| Saber M1 REF       | ref Level 1 | ref Level 3 | ref Level 5 |
|--------------------|------------:|------------:|------------:|
| GenMatrix          |       8,908 |      20,156 |      34,983 |
| GenSecret          |       4,376 |       4,444 |       4,557 |
| crypto_kem_keypair |      44,058 |      74,342 |     119,258 |
| crypto_kem_enc     |      50,850 |      90,407 |     141,025 |
| crypto_kem_dec     |      54,837 |      96,190 |     150,844 |
| InnerProd          |       8,012 |      11,825 |      16,064 |
| MatrixVectorMul    |      16,003 |      35,496 |      64,341 |
