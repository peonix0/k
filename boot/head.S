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

    bl _print_el

    // Set exception vector base
    adrp    x0, _vectors
    add     x0, x0, :lo12:_vectors
    msr     VBAR_EL1, x0
    isb

    // call C: kmain(x0=dtb stays whatever it was; we don’t use it yet)
    bl      kmain

//------ JUST FOR SHOW-------------
_print_el:
    movz    x1, #0x900, lsl #16
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

    movz    x1, #0x900, lsl #16
    strb    w0, [x1]

    movz    w0, #'\n'
    strb    w0, [x1]
    ret

// ------ .rodata ---------------
    .section .rodata.boot, "a"
    .align 3
__el_msg:
   .asciz " not using "
