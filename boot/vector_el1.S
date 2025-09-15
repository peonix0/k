// boot/vectors_el1.S
//------------ Exception vectors ----------------

    .section .text.vectors, "ax"
    .balign 2048
    .globl _vectors

_vectors:
    // Using SP_EL0
    .balign 128             // base + 0x000
    b       _sync_el1t       // Synchronous exceptions

    .balign 128             // base + 0x080
    b       _fiq_el1t        // Fast interrupt

    .balign 128             // base + 0x100
    b       _irq_el1t        // Normal interrupt

    .balign 128             // base + 0x180
    b      _serr_el1t        // SErr exceptions


    // Using SP_ELx
    .balign 128
    b       _sync_el1h

    .balign 128
    b       _fiq_el1h

    .balign 128
    b       _irq_el1h

    .balign 128
    b       _serr_el1h

    // Taken from Lower EL and aarch64
    .balign 128
    b       _hang

    .balign 128
    b       _hang

    .balign 128
    b       _hang

    .balign 128
    b       _hang


    // Taken from Lower EL and aarch32
    .balign 128
    b       _hang

    .balign 128
    b       _hang

    .balign 128
    b       _hang

    .balign 128
    b       _hang

//---------------- Handlers ------------------
    .section .text.vector_handlers, "ax"
    //.global _sync_el1h, _fiq_el1h, _irq_el1h, _serr_el1h
    //.global _sync_el1t, _irq_el1t, _fiq_el1t, _serr_el1t

    // Prototypes for C handlers we'll call
    .extern sync_handler_el1
    .extern irq_handler_el1
    .extern fiq_handler_el1
    .extern serr_handler_el1

_sync_el1h:
    //bl _save_regs
    mrs x0, esr_el1         // Syndrome register
    mrs x1, elr_el1         // Link register
    mrs x2, spsr_el1        // Saved program status register
    mrs x3, far_el1         // Fault address register
    bl sync_handler_el1
    //bl _restore_regs
    eret

_irq_el1h:
    mrs x0, esr_el1
    mrs x1, elr_el1
    mrs x2, spsr_el1
    mrs x3, far_el1
    bl irq_handler_el1
    eret

_fiq_el1h:
    mrs x0, esr_el1
    mrs x1, elr_el1
    mrs x2, spsr_el1
    mrs x3, far_el1
    bl  fiq_handler_el1
    eret

_serr_el1h:
    mrs x0, esr_el1
    mrs x1, elr_el1
    mrs x2, spsr_el1
    mrs x3, far_el1
    bl  serr_handler_el1
    eret

_sync_el1t:  b _hang
_irq_el1t:   b _hang
_fiq_el1t:   b _hang
_serr_el1t:  b _hang

_hang:
    wfe
    b       _hang
