// boot/head.S
    .section .text.boot, "ax"
    .globl _start
    .align  7
_start:
    msr     SPSel, #1                 // use SP_ELx
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
    // (optional) set a dummy vector base
    ldr     x3, =_vectors
    msr     VBAR_EL1, x3
    isb

    // call C: kmain(x0=dtb stays whatever it was; we don’t use it yet)
    bl      kmain

hang:
    wfe
    b       hang

    .balign 2048
_vectors:
    .rept 16
    b hang
    .endr

