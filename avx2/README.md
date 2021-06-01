# AVX2 code benchmark result 

All units are clock cycles. I turn off all `meltdown, spectre` mitigations. Set `sudo cpupower frequency-set --governor performance`. 

```json
$ lscpu
Architecture:                    x86_64
CPU op-mode(s):                  32-bit, 64-bit
Byte Order:                      Little Endian
Address sizes:                   39 bits physical, 48 bits virtual
CPU(s):                          12
On-line CPU(s) list:             0-11
Thread(s) per core:              2
Core(s) per socket:              6
Socket(s):                       1
NUMA node(s):                    1
Vendor ID:                       GenuineIntel
CPU family:                      6
Model:                           158
Model name:                      Intel(R) Core(TM) i7-8750H CPU @ 2.20GHz
Stepping:                        10
CPU MHz:                         4037.065
CPU max MHz:                     4100.0000
CPU min MHz:                     800.0000
BogoMIPS:                        4401.32
Virtualization:                  VT-x
L1d cache:                       192 KiB
L1i cache:                       192 KiB
L2 cache:                        1.5 MiB
L3 cache:                        9 MiB
NUMA node0 CPU(s):               0-11
Vulnerability Itlb multihit:     KVM: Mitigation: VMX disabled
Vulnerability L1tf:              Mitigation; PTE Inversion; VMX vulnerable
Vulnerability Mds:               Vulnerable; SMT vulnerable
Vulnerability Meltdown:          Vulnerable
Vulnerability Spec store bypass: Vulnerable
Vulnerability Spectre v1:        Vulnerable: __user pointer sanitization and usercopy barriers only; no swapg
                                 s barriers
Vulnerability Spectre v2:        Vulnerable, IBPB: disabled, STIBP: disabled
Vulnerability Srbds:             Vulnerable
Vulnerability Tsx async abort:   Not affected
Flags:                           fpu vme de pse tsc msr pae mce cx8 apic sep mtrr pge mca cmov pat pse36 clfl
                                 ush dts acpi mmx fxsr sse sse2 ss ht tm pbe syscall nx pdpe1gb rdtscp lm con
                                 stant_tsc art arch_perfmon pebs bts rep_good nopl xtopology nonstop_tsc cpui
                                 d aperfmperf pni pclmulqdq dtes64 monitor ds_cpl vmx est tm2 ssse3 sdbg fma 
                                 cx16 xtpr pdcm pcid sse4_1 sse4_2 x2apic movbe popcnt tsc_deadline_timer aes
                                  xsave avx f16c rdrand lahf_lm abm 3dnowprefetch cpuid_fault epb invpcid_sin
                                 gle ssbd ibrs ibpb stibp tpr_shadow vnmi flexpriority ept vpid ept_ad fsgsba
                                 se tsc_adjust bmi1 avx2 smep bmi2 erms invpcid mpx rdseed adx smap clflushop
                                 t intel_pt xsaveopt xsavec xgetbv1 xsaves dtherm ida arat pln pts hwp hwp_no
                                 tify hwp_act_window hwp_epp md_clear flush_l1d
```

## Kyber 

- Run `make speed; make bench` to perform benchmark.

| Kyber i7-8750H AVX2            | avx2 Level 1 | avx2 Level 3 | avx2 Level 5 |
|--------------------------------|-------------:|-------------:|-------------:|
| gen_matrix                     |        3,790 |       10,608 |       15,345 |
| poly_getnoise_eta1             |        1,913 |        1,320 |        1,267 |
| poly_getnoise_eta2             |        1,317 |        1,360 |        1,268 |
| poly_getnoise_eta1_4x          |        2,454 |        1,228 |        1,188 |
| poly_ntt                       |          125 |          126 |          126 |
| poly_invntt_tomont             |          146 |          134 |          132 |
| polyvec_basemul_acc_montgomery |          129 |          168 |          218 |
| crypto_kem_keypair             |       14,113 |       23,553 |       32,033 |
| crypto_kem_enc                 |       23,337 |       32,641 |       44,871 |
| crypto_kem_dec                 |       17,118 |       25,226 |       35,925 |
| VectorVectorMul                |          504 |          664 |          878 |
| MatrixVectorMul                |          741 |        1,235 |        2,029 |


## NTRU

- Run `make test/speed; test/speed` to perform benchmark.

| NTRU i7-8750H AVX2         | avx2 HPS509 | avx2 HPS677 | avx2 HRSS701 | avx2 HPS821 |
|----------------------------|-------------|-------------|--------------|-------------|
| crypto_kem_enc             |      35,452 |      47,548 |       27,148 |      58,441 |
| crypto_kem_dec             |      21,624 |      32,387 |       32,670 |      41,784 |
| poly_Rq_mul                |       3,880 |       6,034 |        6,033 |       8,749 |
| poly_S3_mul                |       3,971 |       6,253 |        6,232 |       9,220 |
| randombytes                |      24,091 |      30,868 |       14,076 |      36,481 |
| randombytes                |      24,307 |      30,527 |       14,079 |      36,316 |
| sample_iid                 |         192 |         295 |          250 |         305 |
| sample_fixed_type          |       3,087 |       4,323 |        1,219 |       5,199 |
| poly_lift                  |         145 |         105 |          619 |         158 |
| poly_Rq_to_S3              |         177 |         237 |          240 |         298 |
| poly_Rq_sum_zero_tobytes   |         573 |         762 |          888 |         418 |
| poly_Rq_sum_zero_frombytes |         858 |       1,135 |        1,271 |         799 |
| poly_S3_tobytes            |         240 |         320 |          336 |         390 |
| poly_S3_frombytes          |         444 |         591 |          621 |         705 |

## Saber 

- Run `make speed; make bench` to perform benchmark.

| Saber i7-8750H AVX2 | avx2 Level 1 | avx2 Level 3 | avx2 Level 5 |
|---------------------|-------------:|-------------:|-------------:|
| GenMatrix           |        7,793 |       17,907 |       31,237 |
| GenSecret           |        3,825 |        4,085 |        4,158 |
| crypto_kem_keypair  |       26,077 |       43,280 |       64,917 |
| crypto_kem_enc      |       30,599 |       51,003 |       75,724 |
| crypto_kem_dec      |       29,367 |       49,381 |       73,656 |



## Saber NTT 

- Run `make speed; make bench` to perform benchmark. 

| Saber-NTT i7-8750H AVX2        | avx2 Level 1 | avx2 Level 3 | avx2 Level 5 |
|--------------------------------|-------------:|-------------:|-------------:|
| InnerProd                      |        2,206 |        3,485 |        4,510 |
| InnerProd-NTT                  |        1,762 |        2,416 |        3,063 |
| MatrixVectorMul                |        3,903 |        8,226 |       13,731 |
| MatrixVectorMul-NTT            |        2,854 |        5,621 |        8,784 |
| poly_ntt                       |          157 |          158 |          155 |
| poly_invntt_tomont             |          142 |          141 |          138 |
| poly_basemul_montgomery        |           38 |           38 |           38 |
| polyvec_basemul_acc_montgomery |           59 |           79 |          112 |
| poly_crt                       |            0 |            0 |            0 |
| ntt                            |          989 |        1,023 |          990 |

Note, names changed to match functionality in the paper:

- `saber_iprod = InnerProd`
- `saber_matrix_vector_mul = MatrixVectorMul`
- `polyvec_iprod = InnerProd-NTT`
- `polyvec_matrix_vector_mul = MatrixVectorMul-NTT`
