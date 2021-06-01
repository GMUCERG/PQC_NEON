from params import *

from K2_K2_64x52 import K2_K2_transpose_64x52

p = print


def karatsuba_eval(dst, dst_off, coeff, src, t0, t1):
    """ t1 can overlap with any source register, but not t0 """
    p("vmovdqa %ymm{}, {}({})".format(src[0], (dst_off+4*0+coeff)*32, dst))  # a[0:]
    p("vmovdqa %ymm{}, {}({})".format(src[1], (dst_off+4*1+coeff)*32, dst))  # a[44:]
    p("vpaddw %ymm{}, %ymm{}, %ymm{}".format(src[0], src[1], t0))

    p("vmovdqa %ymm{}, {}({})".format(t0, (dst_off+4*2+coeff)*32, dst))  # s1[0:]
    p("vmovdqa %ymm{}, {}({})".format(src[2], (dst_off+4*3+coeff)*32, dst))  # a[88:]
    p("vmovdqa %ymm{}, {}({})".format(src[3], (dst_off+4*4+coeff)*32, dst))  # a[132:]
    p("vpaddw %ymm{}, %ymm{}, %ymm{}".format(src[2], src[3], t0))

    p("vmovdqa %ymm{}, {}({})".format(t0, (dst_off+4*5+coeff)*32, dst))  # s2[0:]
    p("vpaddw %ymm{}, %ymm{}, %ymm{}".format(src[0], src[2], t0))

    p("vmovdqa %ymm{}, {}({})".format(t0, (dst_off+4*6+coeff)*32, dst))  # s0[0:]
    p("vpaddw %ymm{}, %ymm{}, %ymm{}".format(src[1], src[3], t1))

    p("vmovdqa %ymm{}, {}({})".format(t1, (dst_off+4*7+coeff)*32, dst))  # s0[44:]
    p("vpaddw %ymm{}, %ymm{}, %ymm{}".format(t0, t1, t0))

    p("vmovdqa %ymm{}, {}({})".format(t0, (dst_off+4*8+coeff)*32, dst))  # s3[0:]


def karatsuba_interpolate(dst, dst_off, src, src_off, coeff):
    """ Destroys all ymm regs and does not leave useful values.
    In practice we're doing 7 of these sequentially, so there is no reasonable
    way to save any high-coefficients results. """

    def addr(i, off):
        return '{}({})'.format((src_off+4*(2*i+off//52)+coeff)*32, src)

    r0_52 = 0
    p("vmovdqa {}, %ymm{}".format(addr(0, 52), r0_52))
    out0_52 = r0_52
    p("vpsubw {}, %ymm{}, %ymm{}".format(addr(1, 0), r0_52, out0_52))
    r2_52 = 1
    p("vmovdqa {}, %ymm{}".format(addr(2, 52), r2_52))
    out1_0 = r2_52
    p("vpsubw %ymm{}, %ymm{}, %ymm{}".format(out0_52, r2_52, out1_0))
    p("vpsubw {}, %ymm{}, %ymm{}".format(addr(1, 52), out1_0, out1_0))
    p("vpsubw {}, %ymm{}, %ymm{}".format(addr(0, 0), out0_52, out0_52))
    p("vpaddw {}, %ymm{}, %ymm{}".format(addr(2, 0), out0_52, out0_52))

    r3_52 = 2
    p("vmovdqa {}, %ymm{}".format(addr(3, 52), r3_52))
    out2_52 = r3_52
    p("vpsubw {}, %ymm{}, %ymm{}".format(addr(4, 0), r3_52, out2_52))
    r5_52 = 3
    p("vmovdqa {}, %ymm{}".format(addr(5, 52), r5_52))
    out3_0 = r5_52
    p("vpsubw %ymm{}, %ymm{}, %ymm{}".format(out2_52, r5_52, out3_0))
    p("vpsubw {}, %ymm{}, %ymm{}".format(addr(4, 52), out3_0, out3_0))
    p("vpsubw {}, %ymm{}, %ymm{}".format(addr(3, 0), out2_52, out2_52))
    p("vpaddw {}, %ymm{}, %ymm{}".format(addr(5, 0), out2_52, out2_52))

    r6_52 = 4
    p("vmovdqa {}, %ymm{}".format(addr(6, 52), r6_52))
    p("vpsubw {}, %ymm{}, %ymm{}".format(addr(7, 0), r6_52, r6_52))
    r8_52 = 5
    p("vmovdqa {}, %ymm{}".format(addr(8, 52), r8_52))
    r7_0 = r8_52
    p("vpsubw %ymm{}, %ymm{}, %ymm{}".format(r6_52, r8_52, r7_0))
    p("vpsubw {}, %ymm{}, %ymm{}".format(addr(7, 52), r7_0, r7_0))
    p("vpsubw {}, %ymm{}, %ymm{}".format(addr(6, 0), r6_52, r6_52))
    p("vpaddw {}, %ymm{}, %ymm{}".format(addr(8, 0), r6_52, r6_52))

    p("vpsubw {}, %ymm{}, %ymm{}".format(addr(3, 0), out1_0, out1_0))
    out2_0 = r7_0
    p("vpsubw %ymm{}, %ymm{}, %ymm{}".format(out1_0, r7_0, out2_0))
    p("vpsubw %ymm{}, %ymm{}, %ymm{}".format(out3_0, out2_0, out2_0))
    p("vpsubw {}, %ymm{}, %ymm{}".format(addr(0, 0), out1_0, out1_0))
    p("vpaddw {}, %ymm{}, %ymm{}".format(addr(6, 0), out1_0, out1_0))

    r1_52 = 6
    p("vmovdqa {}, %ymm{}".format(addr(1, 52), r1_52))
    out1_52 = 7
    p("vpsubw %ymm{}, %ymm{}, %ymm{}".format(out2_52, r1_52, out1_52))
    r7_52 = out2_52
    p("vmovdqa {}, %ymm{}".format(addr(7, 52), r7_52))
    p("vpsubw %ymm{}, %ymm{}, %ymm{}".format(out1_52, r7_52, out2_52))
    p("vpsubw {}, %ymm{}, %ymm{}".format(addr(4, 52), out2_52, out2_52))
    p("vpsubw %ymm{}, %ymm{}, %ymm{}".format(out0_52, out1_52, out1_52))
    p("vpaddw %ymm{}, %ymm{}, %ymm{}".format(r6_52, out1_52, out1_52))

    # TODO can get rid of these by fetching them from the right place during Toom4 eval
    out0_0 = 8
    out3_52 = 9
    p("vmovdqa {}, %ymm{}".format(addr(0, 0), out0_0))
    p("vmovdqa {}, %ymm{}".format(addr(4, 52), out3_52))

    # TODO should move these up in between computations for better pipelining?
    p("vmovdqa %ymm{}, {}({})".format(out0_0,  (dst_off+2*0+0)*32, dst))
    p("vmovdqa %ymm{}, {}({})".format(out0_52, (dst_off+2*0+1)*32, dst))
    p("vmovdqa %ymm{}, {}({})".format(out1_0,  (dst_off+2*1+0)*32, dst))
    p("vmovdqa %ymm{}, {}({})".format(out1_52, (dst_off+2*1+1)*32, dst))
    p("vmovdqa %ymm{}, {}({})".format(out2_0,  (dst_off+2*2+0)*32, dst))
    p("vmovdqa %ymm{}, {}({})".format(out2_52, (dst_off+2*2+1)*32, dst))
    p("vmovdqa %ymm{}, {}({})".format(out3_0,  (dst_off+2*3+0)*32, dst))
    p("vmovdqa %ymm{}, {}({})".format(out3_52, (dst_off+2*3+1)*32, dst))


def idx2off(i):
    """ Produces
    [0, 32, 64, 96,   104, 136, 168, 200,   208, 240, 272, 304,   312, 344, 376, 408]
    These are the byte offsets when dividing into 52-coeff chunks"""
    return i * 32 - (24 * (i//4))


if __name__ == '__main__':
    p(".data")
    p(".p2align 5")

    p("mask_low9words:")
    for i in [65535]*9 + [0]*7:
        p(".word 0x{:x}".format(i))

    p("const3:")
    for i in range(16):
        p(".word 3")

    p("const9:")
    for i in range(16):
        p(".word 9")

    p("const0:")
    for i in range(16):
        p(".word 0")

    p("const729:")
    for i in range(16):
        p(".word 729")

    p("const3_inv:")  # inverse of 3 mod 2**16
    for i in range(16):
        p(".word 43691")

    p("const5_inv:")  # inverse of 3 mod 2**16
    for i in range(16):
        p(".word 52429")

    p("rol_rol_16:")
    for j in range(2):
        for i in range(16):
            p(".byte {}".format((i + 2) % 16))

    p("mask32_to_16:")
    for a, b in zip([65535]*8, [0]*8):
        p(".word 0x{:x}".format(a))
        p(".word 0x{:x}".format(b))

    p("mask_9_7:")
    for i in range(9):
        p(".word 65535")
    for i in range(7):
        p(".word 0")

    p("mask_7_9:")
    for i in range(7):
        p(".word 65535")
    for i in range(9):
        p(".word 0")

    p(".text")
    p(".global {}poly_Rq_mul".format(NAMESPACE))
    p(".global _{}poly_Rq_mul".format(NAMESPACE))

    p("{}poly_Rq_mul:".format(NAMESPACE))
    p("_{}poly_Rq_mul:".format(NAMESPACE))
    # assume a and b in rsi and rdx respectively
    # assume destination pointer in rdi
    r_real = '%rdi'
    a_real = '%rsi'
    b_real = '%rdx'

    # karatsuba layers use registers rcx, r9 and r10
    # r8 is used to store the stack pointer
    # that leaves rax and r11 for pointers, so we must preserve one more
    p("push %r12")
    r_out = '%r12'
    a_prep = '%rax'
    b_prep = '%r11'

    p("mov %rsp, %r8")  # Use r8 to store the old stack pointer during execution.
    p("andq $-32, %rsp")  # Align rsp to the next 32-byte value, for vmovdqa.

    # allocate destination block for prepared a
    p("subq ${}, %rsp".format((64 * 64 // 16) * 32))
    p("mov %rsp, {}".format(a_prep))
    # allocate destination block for prepared b
    p("subq ${}, %rsp".format((64 * 64 // 16) * 32))
    p("mov %rsp, {}".format(b_prep))
    # allocate destination block for resulting r
    p("subq ${}, %rsp".format((64 * 128 // 16) * 32))
    p("mov %rsp, {}".format(r_out))

    # allocate some space for f0-f3
    p("subq ${}, %rsp".format(16 * 32))

    # Zero the result register
    p("vpxor %ymm3, %ymm3, %ymm3")
    for i in range(2*832//32):
        p("vmovdqa %ymm3, {}({})".format(i*32, r_real))

    ###### evaluate Toom4 / K2 / K2
    # think of blocks of 52 coefficients, for karatsuba preparation
    # we evaluate for first 16 coefficients of each block, then 16, then 16, then 4

    const_3 = 3
    p("vmovdqa const3(%rip), %ymm{}".format(const_3))

    for (prep, real) in [(a_prep, a_real), (b_prep, b_real)]:
        for coeff in range(4):
            f0 = [0, 1, 2, 12]  # we already have const_3 in 3 (keeping it saves 5 loads)
            # TODO replace vmovdqu with vmovdqa when possible
            for i, r in enumerate(f0):
                p("vmovdqu {}({}), %ymm{}".format(0*13*32+idx2off(i*4+coeff), real, r))

            f3 = [4, 5, 6, 7]
            for i, r in enumerate(f3):
                p("vmovdqu {}({}), %ymm{}".format(3*13*32+idx2off(i*4+coeff), real, r))
            # there are 821 coefficients, not 832;
            if coeff == 2: # Mask out last 7 coeff in load from offset 812
                p("vpand mask_9_7(%rip), %ymm{}, %ymm{}".format(f3[3], f3[3]))
            if coeff == 3: # Mask out last 16 coeff in load from offset 828
                p("vpxor %ymm{}, %ymm{}, %ymm{}".format(f3[3], f3[3], f3[3]))

            # retrieve f1 so we can store it in the stack and use for vpadd
            f1 = [8, 9, 10, 11]
            for i, r in enumerate(f1):
                p("vmovdqu {}({}), %ymm{}".format(1*13*32+idx2off(i*4+coeff), real, r))

            t0 = 14
            t1 = 15
            karatsuba_eval(prep, dst_off=0*9*4, src=f0, t0=t0, t1=t1, coeff=coeff)
            karatsuba_eval(prep, dst_off=6*9*4, src=f3, t0=t0, t1=t1, coeff=coeff)

            # store f0 and f1 so we can use those registers (storing guarantees alignment)
            for i, r in enumerate(f0):
                p("vmovdqa %ymm{}, {}(%rsp)".format(r, (0*4+i)*32))
            for i, r in enumerate(f1):
                p("vmovdqa %ymm{}, {}(%rsp)".format(r, (1*4+i)*32))

            x1 = [8, 9, 10, 11]
            x2 = [12, 13, 14, 15]

            for i in range(4):
                f2_i = 0
                p("vmovdqu {}({}), %ymm{}".format(2*13*32+idx2off(i*4+coeff), real, f2_i))
                f0f2_i = 1
                p("vpaddw {}(%rsp), %ymm{}, %ymm{}".format((0*4+i)*32, f2_i, f0f2_i))
                f1f3_i = 2
                p("vpaddw {}(%rsp), %ymm{}, %ymm{}".format((1*4+i)*32, f3[i], f1f3_i))
                p("vpaddw %ymm{}, %ymm{}, %ymm{}".format(f1f3_i, f0f2_i, x1[i]))
                p("vpsubw %ymm{}, %ymm{}, %ymm{}".format(f1f3_i, f0f2_i, x2[i]))
                # also store the retrieved element of f2 on the stack, makes addition easier later
                p("vmovdqa %ymm{}, {}(%rsp)".format(f2_i, (2*4+i)*32))

            t0 = 0
            t1 = 1
            karatsuba_eval(prep, dst_off=1*9*4, src=x1, t0=t0, t1=t1, coeff=coeff)
            karatsuba_eval(prep, dst_off=2*9*4, src=x2, t0=t0, t1=t1, coeff=coeff)

            x3 = [8, 9, 10, 11]
            x4 = [12, 13, 14, 15]

            for i in range(4):
                f2_i = 0
                p("vmovdqa {}(%rsp), %ymm{}".format((2*4+i)*32, f2_i))
                f2_4_i = 0
                p("vpsllw $2, %ymm{}, %ymm{}".format(f2_i, f2_4_i))
                f0f2_4_i = 0
                p("vpaddw {}(%rsp), %ymm{}, %ymm{}".format((0*4+i)*32, f2_4_i, f0f2_4_i))
                f3_4_i = 1
                p("vpsllw $2, %ymm{}, %ymm{}".format(f3[i], f3_4_i))
                f1f3_4_i = 1
                p("vpaddw {}(%rsp), %ymm{}, %ymm{}".format((1*4+i)*32, f3_4_i, f1f3_4_i))
                f1_2f3_8_i = 1
                p("vpsllw $1, %ymm{}, %ymm{}".format(f1f3_4_i, f1_2f3_8_i))
                p("vpaddw %ymm{}, %ymm{}, %ymm{}".format(f1_2f3_8_i, f0f2_4_i, x3[i]))
                p("vpsubw %ymm{}, %ymm{}, %ymm{}".format(f1_2f3_8_i, f0f2_4_i, x4[i]))

            t0 = 0
            t1 = 1
            karatsuba_eval(prep, dst_off=3*9*4, src=x3, t0=t0, t1=t1, coeff=coeff)
            karatsuba_eval(prep, dst_off=4*9*4, src=x4, t0=t0, t1=t1, coeff=coeff)

            x5 = [12, 13, 14, 15]

            for i in range(4):
                f3_3_i = 0
                p("vpmullw %ymm{}, %ymm{}, %ymm{}".format(const_3, f3[i], f3_3_i))
                f2f3_3_i = 0
                p("vpaddw {}(%rsp), %ymm{}, %ymm{}".format((2*4+i)*32, f3_3_i, f2f3_3_i))
                f2_3f3_9_i = 0
                p("vpmullw %ymm{}, %ymm{}, %ymm{}".format(const_3, f2f3_3_i, f2_3f3_9_i))
                f1f2_3f3_9_i = 0
                p("vpaddw {}(%rsp), %ymm{}, %ymm{}".format((1*4+i)*32, f2_3f3_9_i, f1f2_3f3_9_i))
                f1_3f2_9f3_27_i = 0
                p("vpmullw %ymm{}, %ymm{}, %ymm{}".format(const_3, f1f2_3f3_9_i, f1_3f2_9f3_27_i))
                p("vpaddw {}(%rsp), %ymm{}, %ymm{}".format((0*4+i)*32, f1_3f2_9f3_27_i, x5[i]))

            karatsuba_eval(prep, dst_off=5*9*4, src=x5, t0=t0, t1=t1, coeff=coeff)

    K2_K2_transpose_64x52(r_out, a_prep, b_prep)

    ###### interpolate Toom4 / K2 / K2

    # we could have probably left something in registers after the transpose
    #   but that is extremely messy and would've maybe saved ten cycles at most

    # we need 7*8 registers for the 7 interpolationts.
    #   (2*832/52 = 32, 4-way sequential over coefficients => 8 registers)
    # there are already 16 registers available from f0-f3, so allocate 7*8 - 16 more
    p("subq ${}, %rsp".format((7*8 - 16) * 32))

    registers = list(range(16))

    def free(*regs):
        for x in regs:
            if x in registers:
                raise Exception("This register is already freed")
            registers.append(x)

    def alloc():
        return registers.pop()

    const729 = alloc()
    p("vmovdqa const729(%rip), %ymm{}".format(const729))
    const3_inv = alloc()
    p("vmovdqa const3_inv(%rip), %ymm{}".format(const3_inv))
    const5_inv = alloc()
    p("vmovdqa const5_inv(%rip), %ymm{}".format(const5_inv))
    const9 = alloc()
    p("vmovdqa const9(%rip), %ymm{}".format(const9))

    # consider swapping this around for more closely linked memory access
    # they're somewhat spread around because of how the transpose worked, but
    #   staying sane while incrementally writing/testing this is also important
    for coeff in range(4):
        for i in range(7):
            karatsuba_interpolate(dst='%rsp', dst_off=i*4*2, src=r_out, src_off=i*9*8, coeff=coeff)

        # after interpolating, we can even go 24-way sequential;
        # none of the 52-coefficient chunks interact anymore before reduction

        for j in range(8): # for each 16 (or 4) coefficient chunk
            def limb(i):
                # TODO see above; for case j in {0, 8}, make an exception
                return '{}(%rsp)'.format((i*8+j)*32)

            h0 = alloc()
            p("vmovdqa {}, %ymm{}".format(limb(0), h0))
            h0lo = alloc()
            h0hi = alloc()
            p("vpunpcklwd const0(%rip), %ymm{}, %ymm{}".format(h0, h0lo))
            p("vpunpckhwd const0(%rip), %ymm{}, %ymm{}".format(h0, h0hi))
            free(h0lo)
            h0_2lo = alloc()
            p("vpslld $1, %ymm{}, %ymm{}".format(h0lo, h0_2lo))
            free(h0hi)
            h0_2hi = alloc()
            p("vpslld $1, %ymm{}, %ymm{}".format(h0hi, h0_2hi))

            t1 = alloc()
            p("vmovdqa {}, %ymm{}".format(limb(1), t1))
            t1lo = alloc()
            p("vpunpcklwd const0(%rip), %ymm{}, %ymm{}".format(t1, t1lo))
            free(t1)
            t1hi = alloc()
            p("vpunpckhwd const0(%rip), %ymm{}, %ymm{}".format(t1, t1hi))

            t2 = alloc()
            p("vmovdqa {}, %ymm{}".format(limb(2), t2))
            t2lo = alloc()
            p("vpunpcklwd const0(%rip), %ymm{}, %ymm{}".format(t2, t2lo))
            free(t2)
            t2hi = alloc()
            p("vpunpckhwd const0(%rip), %ymm{}, %ymm{}".format(t2, t2hi))

            t11lo = alloc()
            p("vpaddd %ymm{}, %ymm{}, %ymm{}".format(t2lo, t1lo, t11lo))
            t11hi = alloc()
            p("vpaddd %ymm{}, %ymm{}, %ymm{}".format(t2hi, t1hi, t11hi))

            free(h0_2lo, t11lo)
            t11c1lo = alloc()
            p("vpsubd %ymm{}, %ymm{}, %ymm{}".format(h0_2lo, t11lo, t11c1lo))
            free(h0_2hi, t11hi)
            t11c1hi = alloc()
            p("vpsubd %ymm{}, %ymm{}, %ymm{}".format(h0_2hi, t11hi, t11c1hi))

            free(t1lo, t2lo)
            t12lo = alloc()
            p("vpsubd %ymm{}, %ymm{}, %ymm{}".format(t2lo, t1lo, t12lo))
            free(t1hi, t2hi)
            t12hi = alloc()
            p("vpsubd %ymm{}, %ymm{}, %ymm{}".format(t2hi, t1hi, t12hi))
            p("vpsrld $1, %ymm{}, %ymm{}".format(t12lo, t12lo))
            p("vpsrld $1, %ymm{}, %ymm{}".format(t12hi, t12hi))

            p("vpand mask32_to_16(%rip), %ymm{}, %ymm{}".format(t12lo, t12lo))
            p("vpand mask32_to_16(%rip), %ymm{}, %ymm{}".format(t12hi, t12hi))
            free(t12lo, t12hi)
            r11s = alloc()
            p("vpackusdw %ymm{}, %ymm{}, %ymm{}".format(t12hi, t12lo, r11s))

            h6 = alloc()
            p("vmovdqa {}, %ymm{}".format(limb(6), h6))
            h6lo = alloc()
            p("vpunpcklwd const0(%rip), %ymm{}, %ymm{}".format(h6, h6lo))
            h6hi = alloc()
            p("vpunpckhwd const0(%rip), %ymm{}, %ymm{}".format(h6, h6hi))
            free(h6lo)
            h6_2lo = alloc()
            p("vpslld $1, %ymm{}, %ymm{}".format(h6lo, h6_2lo))
            free(h6hi)
            h6_2hi = alloc()
            p("vpslld $1, %ymm{}, %ymm{}".format(h6hi, h6_2hi))

            free(h6_2lo, t11c1lo)
            t11c2lo = alloc()
            p("vpsubd %ymm{}, %ymm{}, %ymm{}".format(h6_2lo, t11c1lo, t11c2lo))

            free(h6_2hi, t11c1hi)
            t11c2hi = alloc()
            p("vpsubd %ymm{}, %ymm{}, %ymm{}".format(h6_2hi, t11c1hi, t11c2hi))
            p("vpsrld $1, %ymm{}, %ymm{}".format(t11c2lo, t11c2lo))
            p("vpsrld $1, %ymm{}, %ymm{}".format(t11c2hi, t11c2hi))

            p("vpand mask32_to_16(%rip), %ymm{}, %ymm{}".format(t11c2lo, t11c2lo))
            p("vpand mask32_to_16(%rip), %ymm{}, %ymm{}".format(t11c2hi, t11c2hi))
            free(t11c2lo, t11c2hi)
            r11 = alloc()
            p("vpackusdw %ymm{}, %ymm{}, %ymm{}".format(t11c2hi, t11c2lo, r11))

            t3 = alloc()
            p("vmovdqa {}, %ymm{}".format(limb(3), t3))

            t13 = alloc()
            p("vpaddw {}, %ymm{}, %ymm{}".format(limb(4), t3, t13))
            free(t3)
            t14 = alloc()
            p("vpsubw {}, %ymm{}, %ymm{}".format(limb(4), t3, t14))
            free(t14)

            r12s = alloc()
            p("vpsrlw $2, %ymm{}, %ymm{}".format(t14, r12s))
            free(r12s)
            e12s = alloc()
            p("vpsubw %ymm{}, %ymm{}, %ymm{}".format(r11s, r12s, e12s))
            free(e12s)
            r22 = alloc()
            p("vpmullw %ymm{}, %ymm{}, %ymm{}".format(const3_inv, e12s, r22))

            h0_2 = alloc()
            p("vpsllw $1, %ymm{}, %ymm{}".format(h0, h0_2))

            free(t13, h0_2)
            t13c1 = alloc()
            p("vpsubw %ymm{}, %ymm{}, %ymm{}".format(h0_2, t13, t13c1))

            h6_128 = alloc()
            p("vpsllw $7, %ymm{}, %ymm{}".format(h6, h6_128))
            free(t13c1, h6_128)
            t13c2 = alloc()
            p("vpsubw %ymm{}, %ymm{}, %ymm{}".format(h6_128, t13c1, t13c2))
            free(t13c2)
            r12 = alloc()
            p("vpsrlw $3, %ymm{}, %ymm{}".format(t13c2, r12))

            free(r12)
            e12 = alloc()
            p("vpsubw %ymm{}, %ymm{}, %ymm{}".format(r11, r12, e12))
            # currently alive: h0, r11, e12, r11s, r22, h6

            t5 = alloc()
            p("vmovdqa {}, %ymm{}".format(limb(5), t5))

            free(t5)
            t5c1 = alloc()
            p("vpsubw %ymm{}, %ymm{}, %ymm{}".format(h0, t5, t5c1))

            h6_729 = alloc()
            p("vpmullw %ymm{}, %ymm{}, %ymm{}".format(const729, h6, h6_729))

            free(t5c1, h6_729)
            t5c2 = alloc()
            p("vpsubw %ymm{}, %ymm{}, %ymm{}".format(h6_729, t5c1, t5c2))

            free(e12)
            h4 = alloc()
            p("vpmullw %ymm{}, %ymm{}, %ymm{}".format(const3_inv, e12, h4))

            free(r11)
            h2 = alloc()
            p("vpsubw %ymm{}, %ymm{}, %ymm{}".format(h4, r11, h2))

            h4_9 = alloc()
            p("vpmullw %ymm{}, %ymm{}, %ymm{}".format(const9, h4, h4_9))
            # currently alive: h0, h2, h4, h6, h4_9, r22, t5c2, r11s

            free(h4_9)
            h2h4_9 = alloc()
            p("vpaddw %ymm{}, %ymm{}, %ymm{}".format(h4_9, h2, h2h4_9))

            free(h2h4_9)
            h2_9h4_81 = alloc()
            p("vpmullw %ymm{}, %ymm{}, %ymm{}".format(const9, h2h4_9, h2_9h4_81))

            free(t5c2, h2_9h4_81)
            t16 = alloc()
            p("vpsubw %ymm{}, %ymm{}, %ymm{}".format(h2_9h4_81, t5c2, t16))

            free(t16)
            r13 = alloc()
            p("vpmullw %ymm{}, %ymm{}, %ymm{}".format(const3_inv, t16, r13))

            free(r13)
            e13 = alloc()
            p("vpsubw %ymm{}, %ymm{}, %ymm{}".format(r11s, r13, e13))
            free(e13)
            r23 = alloc()
            p("vpsrlw $3, %ymm{}, %ymm{}".format(e13, r23))

            free(r23)
            e23 = alloc()
            p("vpsubw %ymm{}, %ymm{}, %ymm{}".format(r22, r23, e23))

            free(r22)
            h3 = alloc()
            p("vpsubw %ymm{}, %ymm{}, %ymm{}".format(e23, r22, h3))

            free(r11s)
            im1 = alloc()
            p("vpsubw %ymm{}, %ymm{}, %ymm{}".format(h3, r11s, im1))

            free(e23)
            h5 = alloc()
            p("vpmullw %ymm{}, %ymm{}, %ymm{}".format(const5_inv, e23, h5))

            free(im1)
            h1 = alloc()
            p("vpsubw %ymm{}, %ymm{}, %ymm{}".format(h5, im1, h1))
            # currently alive: h0, h1, h2, h3, h4, h5, h6

            h = [h0, h1, h2, h3, h4, h5, h6]

            # TODO replace vmovdqu with vmovdqa when possible (calculate alignment?)
            def get_limb(limbreg, i, j, off=0):
                p("vmovdqu {}({}), %ymm{}".format((off + i*208 + j * 52 + coeff*16) * 2, r_real, limbreg))

            def store_limb(limbreg, i, j, off=0):
                if coeff == 3:
                    if i == 3 and j >= 4:  # this part exceeds 832
                        return
                    p("vmovq %xmm{}, {}({})".format(limbreg, (off + i*208 + j * 52 + coeff*16) * 2, r_real))
                else:
                    if i == 3 and j >= 4:  # this part exceeds 832
                        return
                    p("vmovdqu %ymm{}, {}({})".format(limbreg, (off + i*208 + j * 52 + coeff*16) * 2, r_real))

            tmp = alloc()
            get_limb(tmp, 0, j, off=0)
            p("vpaddw %ymm{}, %ymm{}, %ymm{}".format(tmp, h[0], tmp))
            store_limb(tmp, 0, j, off=0)

            get_limb(tmp, 1, j, off=0)
            p("vpaddw %ymm{}, %ymm{}, %ymm{}".format(tmp, h[1], tmp))
            store_limb(tmp, 1, j, off=0)

            # Handle portions of h[2] that do not wrap around
            if j < 7 or (j == 7 and coeff < 2):
                get_limb(tmp, 2, j, off=0)
                p("vpaddw %ymm{}, %ymm{}, %ymm{}".format(tmp, h[2], tmp))
                store_limb(tmp, 2, j, off=0)

            # Handle portions of h[2] that wrap
            if j == 7 and coeff == 2:
                # Add 9 words at result[812:]
                tmp2 = alloc()
                get_limb(tmp, 2, j, off=0)
                p("vpand mask_9_7(%rip), %ymm{}, %ymm{}".format(h[2], tmp2))
                p("vpaddw %ymm{}, %ymm{}, %ymm{}".format(tmp, tmp2, tmp))
                store_limb(tmp, 2, j, off=0)
                free(tmp2)

                # Add the high 7 words to result[0:]
                #   - rotate left by 1 word in each lane, swap lanes, mask
                p("vpshufb rol_rol_16(%rip), %ymm{}, %ymm{}".format(h[2], h[2]))
                p("vextracti128 $1, %ymm{}, %xmm{}".format(h[2],h[2]))
                p("vpand mask_7_9(%rip), %ymm{}, %ymm{}".format(h[2], h[2]))
                get_limb(tmp, 0, 0, off=(0-16*coeff))
                p("vpaddw %ymm{}, %ymm{}, %ymm{}".format(tmp, h[2], tmp))
                store_limb(tmp, 0, 0, off=(0-16*coeff))

            # Handle portions of h[2] after limb wrap
            if j == 7 and coeff == 3:
                get_limb(tmp, 0, 0, off=(7-16*coeff))
                p("vpaddw %ymm{}, %ymm{}, %ymm{}".format(tmp, h[2], tmp))
                store_limb(tmp, 0, 0, off=(7-16*coeff))

            # Handle portions of h[3] that do not wrap around
            if j < 3 or (j == 3 and coeff < 2):
                get_limb(tmp, 3, j, off=0)
                p("vpaddw %ymm{}, %ymm{}, %ymm{}".format(tmp, h[3], tmp))
                store_limb(tmp, 3, j, off=0)

            # Handle portions of h[3] where limb wraps
            # h[3] holds a segment aligned to result 624:831
            #      starting at 624 + 52*j + (0,16,16,16)[coeff]
            #      and of length (16,16,16,12)[coeff]
            # 821 = 3*208 + 3*52 + 2*16 + 9
            # wrap when j=3, coeff=2
            if j == 3 and coeff == 2:
                # Add 9 words at result[812:]
                tmp2 = alloc()
                get_limb(tmp, 3, j, off=0)
                p("vpand mask_9_7(%rip), %ymm{}, %ymm{}".format(h[3], tmp2))
                p("vpaddw %ymm{}, %ymm{}, %ymm{}".format(tmp, tmp2, tmp))
                store_limb(tmp, 3, j, off=0)
                free(tmp2)

                # Add the high 7 words to result[0:]
                #   - rotate left by 1 word in each lane, swap lanes, mask
                p("vpshufb rol_rol_16(%rip), %ymm{}, %ymm{}".format(h[3], h[3]))
                p("vextracti128 $1, %ymm{}, %xmm{}".format(h[3],h[3]))
                p("vpand mask_7_9(%rip), %ymm{}, %ymm{}".format(h[3], h[3]))
                get_limb(tmp, 0, 0, off=(0-16*coeff))
                p("vpaddw %ymm{}, %ymm{}, %ymm{}".format(tmp, h[3], tmp))
                store_limb(tmp, 0, 0, off=(0-16*coeff))

            # Handle portions of h[3] after limb wrap
            if j == 3 and coeff == 3:
                get_limb(tmp, 0, 0, off=(7-16*coeff))
                p("vpaddw %ymm{}, %ymm{}, %ymm{}".format(tmp, h[3], tmp))
                store_limb(tmp, 0, 0, off=(7-16*coeff))

            if j >= 4:
                get_limb(tmp, 0, j-4, off=11)
                p("vpaddw %ymm{}, %ymm{}, %ymm{}".format(tmp, h[3], tmp))
                store_limb(tmp, 0, j-4, off=11)

            get_limb(tmp, 0, j, off=11)
            p("vpaddw %ymm{}, %ymm{}, %ymm{}".format(tmp, h[4], tmp))
            store_limb(tmp, 0, j, off=11)

            get_limb(tmp, 1, j, off=11)
            p("vpaddw %ymm{}, %ymm{}, %ymm{}".format(tmp, h[5], tmp))
            store_limb(tmp, 1, j, off=11)

            if j < 7 or (j == 7 and coeff < 2):
                get_limb(tmp, 2, j, off=11)
                p("vpaddw %ymm{}, %ymm{}, %ymm{}".format(tmp, h[6], tmp))
                store_limb(tmp, 2, j, off=11)

            free(tmp)
            free(h0, h1, h2, h3, h4, h5, h6)

    p("mov %r8, %rsp")
    p("pop %r12")  # restore callee-saved r12
    p("ret")

