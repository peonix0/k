/* SPDX-License-Identifier: MIT
 * Exception, not of expectation

 * I'm am one of those
*/

#include "common.h"
#include "generic_timer.h"
#include "gic.h"
#include "uart.h"

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
  uart_logs("\n[SYNCH] EL1h\n");

  uart_logb("  ESR:", esr);
  uart_logs("    (EC:");
  uart_logs(exception_class_to_name(ec));
  uart_logs(")\n");

  uart_logx("  ELR: ", elr);
  uart_logx("  FAR: ", far);
  uart_logx("  SPSR: ", spsr);

  debug_backtrace();

  // Hang so we see it
  for (;;) {
  }
}

void irq_handler_el1() { uart_logs("\n[IRQ] EL1h\n"); }

void fiq_handler_el1() {
  u64 reg, iar;
  u32 intid;

  iar = gic_ack();
  intid = iar & 0xFFFFF;

  uart_logs("\n[FIQ] EL1h\n");
  uart_logx(" INTID: ", intid);

  reg = READ_SYSREG64(ICC_RPR_EL1) & 0xF8;
  uart_logx(" ICC_RPR_EL1 [PRE]: ", reg);

  if (intid == 30) {
    /* Generic Timer */

    gicr_dump_info();
    gic_eoi1(iar);

    // TODO: Remove this later
    timer_setp_tval(1u << 30);
  } else if (intid == 33) {
    /* Uart */
    //gicr_dump_info();
    uart_interrupt_handler();
    gic_eoi1(iar);
    uart_logs(" [UART Interrupt Recieved]\n");
  }

  reg = READ_SYSREG64(ICC_RPR_EL1) & 0xF8;
  uart_logx(" ICC_RPR_EL1 [POST]: ", reg);
}
void serr_handler_el1() { uart_logs("[SError]\n"); }
