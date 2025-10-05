/* SPDX-License-Identifier: MIT
 * Never lose it
*/

#include "uart.h"
#include "common.h"
#include "gic.h"

#define UART_RING_SIZE 32
static struct {
  u8 pos, cur;
  u8 data[UART_RING_SIZE];
} ring;

static void uart_ring_push(u8 ch) {
  /* Logging */
  uart_logx(" Recieved Byte: ", ch);
  uart_puts(" Key Pressed: ");
  uart_putc(ch);
  uart_putc('\n');

  ring.data[ring.cur] = ch;
  ring.cur = (ring.cur + 1) % UART_RING_SIZE;
}

static bool uart_ring_pop(u8 *ch) {
  if (ring.pos == ring.cur) {
    return false;
  }

  *ch = ring.data[ring.pos++];
  ring.pos = (ring.pos + 1) % UART_RING_SIZE;

  return true;
}

#if defined(PLATFORM_qemu)
/* ============ QEMU virt: PL011 @ 0x09000000 ============ */
/* We only need:
 *  - FR (offset 0x18): poll TXFF (bit 5) == 0 => space in TX FIFO
 *  - DR (offset 0x00): write a byte to transmit
 */
#define UART_BASE 0x09000000UL
#define UARTDR (UART_BASE + 0x00)
#define UARTFR (UART_BASE + 0x18)
#define UARTCR (UART_BASE + 0x030)
#define UARTIMSC (UART_BASE + 0x038)
#define UARTRIS (UART_BASE + 0x03C)
#define UARTMIS (UART_BASE + 0x040)
#define UARTICR (UART_BASE + 0x044)
#define UARTIFLS (UART_BASE + 0x034)

#define FR_TXFE (1u << 7)
#define FR_TXFF (1u << 5)
#define FR_RXFF (1u << 6)
#define FR_RXFE (1u << 4)

#define CR_UARTEN (1u << 0)

#define UARTIMSC_RXIM (1u << 4)
#define UARTIMSC_RTIM (1u << 6)
#define UARTIMSC_TXIM (1u << 5)

/* Interrupt Clear Register */
#define UARTICR_RTIC (1u << 6)
#define UARTICR_TXIC (1u << 5)
#define UARTICR_RXIC (1u << 4)

/* Masked Interrupt Status */
#define UARTMIS_RXMIS (1u << 4)
#define UARTMIS_TXMIS (1u << 5)
#define UARTMIS_RTMIS (1u << 6)

#define UART_INTID 33u

void uart_init(void) { /* QEMU default config is fine */ }

void uart_putc(char c) {
  while (mmio_r32(UARTFR) & FR_TXFF) {
  }
  mmio_w32(UARTDR, (u32)c);
}

bool uart_tstc() { return !(mmio_r32(UARTFR) & FR_RXFE); }

void uart_readc() {
  if (uart_tstc()) {
    uart_ring_push(mmio_r32(UARTDR) & 0xFF);
  }
}

#elif defined(PLATFORM_rk3588)
/* ============ RK3588: DW APB UART2 (8250-like) @ 0xFEB50000 ============ */
/* Minimal TX needs:
 *  - LSR (word offset 0x05, <<2 -> 0x14): poll THRE (bit 5) == 1
 *  - THR (word offset 0x00, <<2 -> 0x00): write a byte
 * We rely on U-Boot for clocks, pinmux, and baud (usually 1.5M).
 */
#define UART_BASE 0xFEB50000UL
#define REG_SHIFT 2
#define UREG(n) (UART_BASE + ((n) << REG_SHIFT))

#define UART_THR UREG(0x00) // TX holding (write)
#define UART_LSR UREG(0x05) // Line Status
#define LSR_THRE (1u << 5)  // THR empty

void uart_init(void) { /* leave U-Boot settings alone */ }

void uart_putc(char c) {
  while ((mmio_r32(UART_LSR) & LSR_THRE) == 0) {
  }
  mmio_w32(UART_THR, (u32)c);
}

#else
#error "Define PLATFORM=qemu or PLATFORM=rk3588 in your build."
#endif

void uart_enable_interrupt() {
  u64 mpidr_reg;

  // TODO: Maybe disable uart first?
  /* RXIFLSEL[5:3], TXIFLSEL[2:0] */
  /* Generate interrupt when fifo is 7/8 full */
  mmio_w32(UARTIFLS, 0b100100);

  /* Clear all pending interrupts */
  mmio_w32(UARTICR, 0xFFFF);

  /* Clear mask for RX, RT */
  mmio_w32(UARTIMSC, UARTIMSC_RXIM | UARTIMSC_RTIM);

  /* GIC Configurations */
  // TODO: should we? priority
  gic_set_priority(UART_INTID, 0x9F);
  gic_configure_trigger(UART_INTID, 0);
  gic_set_group(UART_INTID, 1);
  gic_set_groupmod(UART_INTID, 1);

  /* Set routing mode to allow to any PE */
  __asm__ __volatile__("mrs %0, MPIDR_EL1" : "=r"(mpidr_reg));
  gicd_set_route(UART_INTID, mpidr_reg & ~(1ull << GICD_IROUTER_MODE_BIT));

  gic_enable_intid(UART_INTID);
}

void uart_interrupt_handler() {
    uart_readc();
}

bool uart_getc(u8 *ch) {
  if (uart_ring_pop(ch)) {
    return true;
  }
  return false;
}

void uart_gets(u8 *buf, u32 bufsize) {
  u32 i = 0;
  u8 ch;

  if (bufsize <= 0)
    return;

  while (i < bufsize - 1) {
    if (uart_getc(&ch))
      buf[i++] = ch;
    else
      break;
  }

  buf[i] = '\0';
}

void uart_puts(const char *s) {
  for (; *s; ++s) {
    if (*s == '\n')
      uart_putc('\r');
    uart_putc(*s);
  }
}

void uart_putb(const char *s, const u64 val) {
  uart_puts(s);

  for (int i = 63; i >= 0; i--) {
    if (val & (1ull << i))
      uart_putc('1');
    else
      uart_putc('0');

    if (!(i & 7)) {
      uart_putc(' ');
    }
  }

  uart_putc('\n');
}

void uart_putx(const u64 val) {
  u8 leadz = 1;
  uart_puts("0x");
  for (int i = 60; i >= 0; i -= 8) {
    char c1 = (val >> i & 0xF);
    char c2 = (val >> (i - 4) & 0xF);

    if (!(c1 || c2) && leadz) {
      continue;
    } else if (leadz) {
      leadz = 0;
    }

    uart_putc(c1 + (c1 < 10 ? '0' : 'A' - 10));
    uart_putc(c2 + (c2 < 10 ? '0' : 'A' - 10));
  }

  if (leadz) {
    uart_puts("00");
  }
}

void uart_logx(const char *s, const u64 val) {
  uart_puts(s);
  uart_putx(val);
  uart_putc('\n');
}
