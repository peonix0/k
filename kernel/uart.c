#include "uart.h"
#include "stdint.h"

static inline void mmio_w(uint64_t a, uint32_t v){ *(volatile uint32_t*)a = v; }
static inline uint32_t mmio_r(uint64_t a){ return *(volatile uint32_t*)a; }

#if defined(PLATFORM_qemu)
/* ============ QEMU virt: PL011 @ 0x09000000 ============ */
/* We only need:
 *  - FR (offset 0x18): poll TXFF (bit 5) == 0 => space in TX FIFO
 *  - DR (offset 0x00): write a byte to transmit
 */
#define UART_BASE  0x09000000UL
#define UART_DR    (UART_BASE + 0x00)
#define UART_FR    (UART_BASE + 0x18)
#define FR_TXFF    (1u << 5)

void uart_init(void) { /* QEMU default config is fine */ }

void uart_putc(char c){
    while (mmio_r(UART_FR) & FR_TXFF) { }
    mmio_w(UART_DR, (uint32_t)c);
}

#elif defined(PLATFORM_rk3588)
/* ============ RK3588: DW APB UART2 (8250-like) @ 0xFEB50000 ============ */
/* Minimal TX needs:
 *  - LSR (word offset 0x05, <<2 -> 0x14): poll THRE (bit 5) == 1
 *  - THR (word offset 0x00, <<2 -> 0x00): write a byte
 * We rely on U-Boot for clocks, pinmux, and baud (usually 1.5M).
 */
#define UART_BASE   0xFEB50000UL
#define REG_SHIFT   2
#define UREG(n)    (UART_BASE + ((n) << REG_SHIFT))

#define UART_THR    UREG(0x00)     // TX holding (write)
#define UART_LSR    UREG(0x05)     // Line Status
#define LSR_THRE    (1u << 5)      // THR empty

void uart_init(void) { /* leave U-Boot settings alone */ }

void uart_putc(char c){
    while ((mmio_r(UART_LSR) & LSR_THRE) == 0) { }
    mmio_w(UART_THR, (uint32_t)c);
}

#else
#error "Define PLATFORM=qemu or PLATFORM=rk3588 in your build."
#endif

void uart_puts(const char* s){
    for (; *s; ++s) {
        if (*s == '\n') uart_putc('\r');
        uart_putc(*s);
    }
}

void uart_putb(const char* s, const uint64_t val) {
    uart_puts(s);

    for(int i=63; i>=0; i--){
        if (val & (1 << i))
            uart_putc('1');
        else
            uart_putc('0');

        if (!(i & 7)) {
            uart_putc(' ');
	}
    }

    uart_putc('\n');
}
