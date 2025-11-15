#include "common.h"
#include "generic_timer.h"
#include "mmu.h"
#include "uart.h"
#include "gic.h"
#include "mm/mem.h"


void run_clock() {
  static u64 ptick = 0;
  static u64 cnt = 0;
  u64 tick;

  tick = READ_SYSREG64(CNTPCT_EL0);

  if ((ptick & (1 << 26)) ^ (tick & (1 << 26))) {
    bool istatus = timer_istatus();
    uart_putx(istatus);
    uart_puts(" /Watch (seconds): ");
    uart_putx(cnt++);
    uart_puts("\r");
  }

  ptick = tick;
}

void ghost16call() {
  /* Set timer TVAL to buzz off after every 16Sec of run */
  /* This will generate a interrupt and wake up the ghost handler */
  uart_puts(" k: setting for ghost16call\n");
  timer_setp_tval(1<<30);
}

void test_memory_exception() {
    volatile u64 *p = (u64*)0xDEADBE000ULL;
    (void)*p;
}

void loop() {
  run_clock();
}

void kmain(void *dtb) {
  (void)dtb;
  u64 ticks = 0;
  u32 tfreq = 0;

  uart_puts("k: hello!\n");

  mmu_enable();
  uart_puts("k: mmu enabled!\n");
  uart_puts("k: mmu: hello!\n");

  uart_puts("k: timer init!\n");
  timer_event_stream_init();

  ghost16call();

  tfreq = timer_get_freq();
  uart_puts(" CNTFREQ_EL0: ");
  uart_putx(tfreq);
  uart_puts("\n");

  ticks = timer_get_vct();
  uart_puts(" CNTVCT_EL0: ");
  uart_putx(ticks);
  uart_puts("\n");

  ticks = timer_get_pct();
  uart_puts(" CNTPCT_EL0: ");
  uart_putx(ticks);
  uart_puts("\n");

  gic_init();
  timer_interrupt_set(true);
  uart_enable_interrupt();

  uart_puts("k: mm init\n");
  mm_init();
  mm_stats();

  for (;;) {
    __asm__ __volatile__("wfe");
    loop();
  }
}
