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
void uart_putb(const char* s, const u64 val);
void uart_putx(const u64 val);
void uart_logx(const char *s, const u64 val);
#endif // __HEADER_UART_H__
