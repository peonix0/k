/* SPDX-License-Identifier: MIT
 * Communication is Key
*/

#ifndef __HEADER_UART_H__
#define __HEADER_UART_H__

#include "common.h"

void uart_init(void);
void uart_enable_interrupt();
void uart_interrupt_handler();

bool uart_getc(u8 *ch);
void uart_gets(u8 *buf, u32 bufsize);

void uart_putc(char c);
void uart_puts(const char* s);
void uart_putb(const u64 val);
void uart_putx(const u64 val);

#ifdef DEBUG
static inline void uart_logs(const char *s) {
    uart_puts(s);
}

static inline void uart_logx(const char *s, u64 val) {
    uart_puts(s);
    uart_putx(val);
    uart_putc('\n');
}

static inline void uart_logb(const char *s, u64 val) {
    uart_puts(s);
    uart_putb(val);
    uart_putc('\n');
}
#else
static inline void uart_logs(const char *s) { (void)s; }
static inline void uart_logx(const char *s, u64 val) { (void)s; (void)val; }
static inline void uart_logb(const char *s, u64 val) { (void)s; (void)val; }
#endif


#endif // __HEADER_UART_H__
