#include "uart.h"

void kmain(void* dtb) {
    (void)dtb;
    uart_init();                 // safe no-op on QEMU unless you enabled baud programming
    uart_puts("k: hello!\n");
    for(;;) __asm__ __volatile__("wfe");
}

