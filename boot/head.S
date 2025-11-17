// boot/head.S
    .section .text.boot, "ax"
    .globl _start
    .align  7

_start:
    bl _print_el

    mrs     x0, CurrentEL
    lsr     x0, x0, #2
    and     x0, x0, #3
    cmp     x0, #1
    beq     _el1

    mov     x0, #(1 << 31)     // HCR_EL2.RW = 1 (EL1 is AArch64)
    msr     HCR_EL2, x0
    isb

    adr     x0, _el1
    msr     ELR_EL2, x0

    ldr     x1, =_stack_top
    msr     SP_EL1, x1

    mov     x0, #(0b0101 | (0xF << 6))
    msr     SPSR_EL2, x0
    isb


    eret

_el1:
    msr     SPSel, #1       // use SP_ELx
    ldr     x0, =_stack_top
    mov     sp, x0

    // clear .bss
    ldr     x1, =__bss_start
    ldr     x2, =__bss_end
1:  cmp     x1, x2
    b.hs    2f
    str     xzr, [x1], #8
    b       1b
2:

    // Set exception vector base
    adrp    x0, _vectors
    add     x0, x0, :lo12:_vectors
    msr     VBAR_EL1, x0
    isb

    // call C: kmain(x0=dtb; we don’t use it yet)
    bl      kmain

//------ JUST FOR SHOW-------------
_print_el:
#ifdef PLATFORM_qemu
    movz    x1, #0x900, lsl #16
#else
    movz    x1, #0xFEB5, lsl #16
#endif
    movz    w0, #'E'
    strb    w0, [x1]

    movz    w0, #'L'
    strb    w0, [x1]

    movz    w0, #':'
    strb    w0, [x1]

    mrs     x0, CurrentEL
    lsr     x0, x0, #2
    and     x0, x0, #3
    add     x0, x0, #48


    strb    w0, [x1]

    movz    w0, #'\n'
    strb    w0, [x1]
    movz    w0, #'\n'
    strb    w0, [x1]

    ret

// ------ .rodata ---------------
    .section .rodata.boot, "a"
    .align 3
__el_msg:
   .asciz " not using "
