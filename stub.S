    .section .text, "ax"
    .globl _start
_start:
    ldr   x16, 1f       // load absolute address (literal placed below)
    br    x16           // branch to it
1:
    .quad  0x0000000040200000  // address where you load k.bin

