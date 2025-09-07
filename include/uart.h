#ifndef __UART_H__
#define __UART_H__
#include <stdint.h>

void uart_init(void);
void uart_putc(char c);
void uart_puts(const char* s);
void uart_putb(const char* s, const uint64_t val);
#endif // __UART_H__
