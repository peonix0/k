#include "uart.h"
#include "mmu.h"

void kmain(void* dtb) {
    (void)dtb;
    uart_init();                 // safe no-op on QEMU unless you enabled baud programming
    uart_puts("k: hello!\n");
    mmu_enable();
    uart_puts("k: mmu enabled!\n");
    uart_puts("k: mmu: hello!\n");
    for(;;) __asm__ __volatile__("wfe");
}

