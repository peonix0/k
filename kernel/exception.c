// kernel/exception.c
#include "uart.h"

static const char *exception_class_to_name(uint8_t ec) {
  switch (ec) {
  case 0b000000:
    return "Unknown";
  case 0b100100:
    return "Data abort, lower EL";
  case 0b100101:
    return "Data abort, same EL";
  case 0b100000:
    return "Instruction abort, lower EL";
  case 0b100001:
    return "Instruction abort, same EL";
  case 0b010101:
    return "SVC (EL0)";
  default:
    return "?";
  }
}

void sync_handler_el1(uint64_t esr, uint64_t elr, uint64_t spsr, uint64_t far) {
  uint8_t ec = (esr >> 26) & 0x3f;
  uart_puts("\n[SYNCH] EL1h\n");

  uart_putb("  ESR:", esr);
  uart_puts("    (EC:");
  uart_puts(exception_class_to_name(ec));
  uart_puts(")\n");

  uart_puts("  ELR:");
  uart_putx(elr);
  uart_puts("\n");
  uart_puts("  FAR:");
  uart_putx(far);
  uart_puts("\n");
  uart_puts("  SPSR:");
  uart_putx(spsr);
  uart_puts("\n");

  // Hang so we see it
  for (;;) {
  }
}

void irq_handler_el1(uint64_t esr, uint64_t elr, uint64_t spsr, uint64_t far) {
  (void)esr;
  (void)elr;
  (void)spsr;
  (void)far;
  uart_puts("[IRQ]\n");
}
void fiq_handler_el1() { uart_puts("[FIQ]\n"); }
void serr_handler_el1() { uart_puts("[SError]\n"); }
