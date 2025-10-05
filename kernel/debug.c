#include "common.h"
#include "uart.h"

#define MAXTRACE 10

struct frame {
  struct frame *prev;
  u64 lr;
} *fp;

static inline struct frame *_read_fp() {
  struct frame *fp;

  __asm__ __volatile__("mov %0, x29" : "=r"(fp));

  return fp;
}

void debug_backtrace() {
  u8 i = 0;
  struct frame *fp;

  uart_puts("\n----------------------------\n");
  uart_puts("   [DEBUG: BACKTRACE]\n");
  uart_puts("----------------------------\n");

  fp = _read_fp();

  while (fp && i < MAXTRACE) {
    if (i++ > 1) {
      uart_logx("[LR]: ", (fp->lr - 4));
    }
    fp = fp->prev;
  }

  uart_puts("----------------------------\n");
}
