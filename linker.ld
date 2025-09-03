/* Linker script for k on QEMU virt (AArch64)
 * Virt RAM base = 0x4000_0000. We link at 0x4020_0000 to leave headroom.
 * Identity-map VA=PA initially, so VMA=LMA is fine.
 */
ENTRY(_start)

SECTIONS {
  . = 0x40200000;

  /* Code first: put bootstrap early for nice, aligned entry */
  .text : ALIGN(16) {
    KEEP(*(.text.boot))        /* your _start in head.S should live here */
    *(.text*)                  /* rest of code */
  }

  .rodata : ALIGN(16) {
    *(.rodata*) *(.const*)
  }

  .data : ALIGN(16) {
    *(.data*)
  }

  /* BSS is NOBITS: occupies memory at runtime, not file size */
  .bss (NOLOAD) : ALIGN(16) {
    __bss_start = .;
    *(.bss*) *(COMMON)
    . = ALIGN(16);
    __bss_end = .;
  }

  /* Simple 8 KiB boot stack (grows down). Expose _stack_* to head.S */
  . = ALIGN(16);
  _stack_bottom = .;
  . += 8192;
  _stack_top = .;
}

