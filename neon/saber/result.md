make: Nothing to be done for `speed'.
cothan@Quans-MBP Cortex-A_Implementation_KEM % make bench
./test/speed512
GenMatrix: 2.81826
GenSecret: 1.36531
crypto_kem_keypair: 9.87502
crypto_kem_enc: 11.478
crypto_kem_dec: 11.1991
neonInnerProd: 0.998471
neonMatrixVectorMul: 2.12551
./test/speed768
GenMatrix: 6.34304
GenSecret: 1.2795
crypto_kem_keypair: 16.1425
crypto_kem_enc: 18.8063
crypto_kem_dec: 18.2395
neonInnerProd: 1.35498
neonMatrixVectorMul: 4.16615
./test/speed1024
GenMatrix: 11.1251
GenSecret: 1.43239
crypto_kem_keypair: 24.3276
crypto_kem_enc: 27.7866
crypto_kem_dec: 27.4185
neonInnerProd: 1.69621
neonMatrixVectorMul: 6.88615


cothan@Quans-MBP Reference_Implementation_KEM % make bench
./test/speed512
GenMatrix: 2.82234
GenSecret: 1.36469
crypto_kem_keypair: 12.7121
crypto_kem_enc: 15.9677
crypto_kem_dec: 17.0413
InnerProd: 3.74314
MatrixVectorMul: 8.83338
./test/speed768
GenMatrix: 6.3884
GenSecret: 1.3962
crypto_kem_keypair: 23.6011
crypto_kem_enc: 28.9044
crypto_kem_dec: 30.8333
InnerProd: 3.84119
MatrixVectorMul: 11.5422
./test/speed1024
GenMatrix: 11.118
GenSecret: 1.43986
crypto_kem_keypair: 38.2927
crypto_kem_enc: 44.9687
crypto_kem_dec: 48.5422
InnerProd: 5.15329
MatrixVectorMul: 20.4513
cothan@Quans-MBP Reference_Implementation_KEM %

./test/speed512
GenMatrix: 2.80856
GenSecret: 1.36525
crypto_kem_keypair: 9.95007
crypto_kem_enc: 11.4977
crypto_kem_dec: 11.2065
neonInnerProd: 1.01287
neonMatrixVectorMul: 2.11683
./test/speed768
GenMatrix: 6.38359
GenSecret: 1.29515
crypto_kem_keypair: 16.2519
crypto_kem_enc: 18.9482
crypto_kem_dec: 18.4158
neonInnerProd: 1.35635
neonMatrixVectorMul: 4.16801
./test/speed1024
GenMatrix: 10.9767
GenSecret: 1.45144
crypto_kem_keypair: 24.5096
crypto_kem_enc: 28.0032
crypto_kem_dec: 27.6087
neonInnerProd: 1.69615
neonMatrixVectorMul: 6.88447
cothan@Quans-MBP Cortex-A_Implementation_KEM %



cothan@Quans-MBP Cortex-A_Implementation_KEM % make bench
./test/speed512
GenMatrix: 2.81113
GenSecret: 1.36505
crypto_kem_keypair: 9.9256
crypto_kem_enc: 11.6192
crypto_kem_dec: 11.1623
neonInnerProd: 1.01426
neonMatrixVectorMul: 2.11714
CLOCKS_PER_SEC: 1000000
./test/speed768
GenMatrix: 6.40899
GenSecret: 1.28241
crypto_kem_keypair: 16.2825
crypto_kem_enc: 18.9024
crypto_kem_dec: 18.3746
neonInnerProd: 1.35504
neonMatrixVectorMul: 4.17191
CLOCKS_PER_SEC: 1000000
./test/speed1024
GenMatrix: 11.1052
GenSecret: 1.43928
crypto_kem_keypair: 24.4688
crypto_kem_enc: 27.9387
crypto_kem_dec: 27.6072
neonInnerProd: 1.69542
neonMatrixVectorMul: 6.88136
CLOCKS_PER_SEC: 1000000