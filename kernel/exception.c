/* SPDX-License-Identifier: MIT
 * Exception, not of expectation

 * I'm am one of those
*/

#include "common.h"
#include "uart.h"
#include "gic.h"
#include "generic_timer.h"

static const char *exception_class_to_name(u8 ec) {
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

void sync_handler_el1(u64 esr, u64 elr, u64 spsr, u64 far) {
  u8 ec = (esr >> 26) & 0x3f;
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

void irq_handler_el1() {
  uart_puts("\n[IRQ] EL1h\n");
}

void fiq_handler_el1() {
  u64 reg, iar;
  u32 intid;

  iar = gic_ack();
  intid = iar & 0xFFFFF;

  uart_puts("\n[FIQ] EL1h\n");
  uart_puts(" INTID: ");
  uart_putx(intid);
  uart_puts("\n");

  reg = READ_SYSREG64(ICC_RPR_EL1) & 0xFF;
  uart_puts(" ICC_RPR_EL1 [PRE]: ");
  uart_putx(reg);
  uart_puts("\n");

  if((u32)intid == 30) {
    /* Generic Timer */
    gic_eoi1(iar);

    // TODO: Remove this later
    timer_setp_tval(1u<<30);
    gicr_dump_info();
  } else if ( intid == 33) {
    /* Uart */
    gic_eoi1(iar);
    uart_puts(" [UART Interrupt Recieved]\n");
  }

  reg = READ_SYSREG64(ICC_RPR_EL1) & 0xFF;
  uart_puts(" ICC_RPR_EL1 [POST]: ");
  uart_putx(reg);
  uart_puts("\n");
}
void serr_handler_el1() { uart_puts("[SError]\n"); }
