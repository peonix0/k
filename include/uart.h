#ifndef __UART_H__
#define __UART_H__

#include "common.h"

void uart_init(void);
void uart_putc(char c);
void uart_puts(const char* s);
void uart_putb(const char* s, const u64 val);
void uart_putx(const u64 val);
#endif // __UART_H__
