#include "mmu.h"
#include "uart.h"

// __attribute__((aligned(4096))) static uint64_t L0[512];
__attribute__((aligned(4096))) static uint64_t L1[512]; // 1GB Each
//__attribute__((aligned(4096))) static uint64_t L2_kernel[512]; // 2 MB Each
__attribute__((aligned(4096))) static uint64_t L2_uart[512];


// --------------------- DUMPING- JUMPING-----------------------------------//
// static void dump_sctlr(uint64_t sctlr) {
//     uart_puts("SCTLR_EL1 = 0x%016llx  [M=%d C=%d I=%d A=%d SA=%d SA0=%d WXN=%d]\n",
//            (unsigned long long)sctlr,
//            (int)((sctlr>>0)&1),   // M: MMU enable
//            (int)((sctlr>>2)&1),   // C: data cache enable
//            (int)((sctlr>>12)&1),  // I: icache enable
//            (int)((sctlr>>1)&1),   // A: alignment check
//            (int)((sctlr>>3)&1),   // SA: SP alignment check EL1
//            (int)((sctlr>>4)&1),   // SA0: SP alignment check EL0
//            (int)((sctlr>>19)&1)); // WXN: write implies XN
// }


static inline uint64_t desc_block_2m(uint64_t pa, int attr_idx) {
    uint64_t d = (pa & ~((1ULL<<21)-1))      // 2MiB aligned output address
               | (1ULL<<10)                  // AF
               | ((uint64_t)attr_idx << 2)   // AttrIndx[4:2]
               | (1 ? (1ULL<<54) : 0)       // UXN
               | (0 ? (1ULL<<53) : 0)       // PXN
               | (1ULL<<0);                  // VALID, TYPE=BLOCK
    // AP bits default 00 (EL1 RW), SH=Inner (3<<8)
    d |= (3ULL<<8);
    return d;
}

static inline uint64_t desc_block_1g(uint64_t pa_page, int attr_idx) {
    uint64_t d = (pa_page & ~((1ULL<<30)-1))
                | (1ULL<<10)
                | ((uint64_t)attr_idx << 2) // AtrrIndx[4:2]
                | (1 ? (1ULL<<54) : 0) // UXN
                | (0 ? (1ULL<<53) : 0) // PXN
                | (1ULL << 0);  // VALID, TYPE=Table Entry

    d |= (3ULL << 8);
    return d;
}

static inline uint64_t desc_page_4k(uint64_t pa_page, int attr_idx) {
    uint64_t e = (pa_page & ~((1ULL<<12)-1))
                | (1ULL<<10)
                | ((uint64_t)attr_idx << 2) // AtrrIndx[4:2]
                | (1 ? (1ULL<<54) : 0) // UXN
                | ( 0 ? (1ULL<<53) : 0) // PXN
                | (3ULL << 0);  // VALID, TYPE=Table Entry

    e |= (3ULL << 8);
    return e;
}

// point to a 4KB page
static inline uint64_t desc_table(uint64_t pa) {
    return (pa & ~0xFFFULL) | (1ULL<<1) | 1ULL; // VALID, TYPE=TABLE
}

static inline uint64_t tcr_value(void){
    const uint64_t T0SZ  = 32ULL;   // 32-bit VA
    const uint64_t TG0   = 0ULL;    // 4KB
    const uint64_t SH0   = 3ULL;    // Inner
    const uint64_t ORGN0 = 1ULL<<10 | 1ULL<<11; // WB WA
    const uint64_t IRGN0 = 1ULL<<8  | 1ULL<<9;  // WB WA
    const uint64_t IPS   = 0b010ULL<<32;        // 48-bit PA
    return (T0SZ) | (TG0<<14) | (SH0<<12) | (IRGN0) | (ORGN0) | IPS;
}

extern char __text_start[]; // pick your kernel base symbol
void mmu_enable(void){
    // map kernel @ 0x40200000 as Normal WB

    uint64_t kva = 0x40200000ULL;
    L1[L1_IDX(kva)] = desc_block_1g(kva, 0x1);
    //L0[L0_IDX(kva)] = desc_table((uint64_t) L1_kernel);

    // Map QEMU PL011 or RK UART as Device (AttrIdx0)
#ifdef PLATFORM_qemu
    uint64_t uva = 0x09000000ULL;
#else
    uint64_t uva = 0xFEB50000ULL;
#endif
    uint64_t l2_uart_entry = desc_block_2m(uva, 0x0);
    L2_uart[L2_IDX(uva)] = l2_uart_entry;
    L1[L1_IDX(uva)] = desc_table((uint64_t) L2_uart);

    uart_puts("mmu: configuring....\n");
    // 2) Program MAIR/TCR/TTBR0
    uint64_t mair = (MAIR_ATTR_DEVnGnRE << 0) | (MAIR_ATTR_NORMALWB << 8);
    __asm__ volatile(
        "msr mair_el1, %0\n"
        "msr tcr_el1,  %1\n"
        "msr ttbr0_el1,%2\n"
        "isb\n"
        "tlbi vmalle1\n"
        "dsb ish\n"
        "isb\n"
        :
        : "r"(mair), "r"(tcr_value()), "r"(L1)
        : "memory");

    // 3) Enable MMU + caches (M|C|I)
    uint64_t sctlr;
    __asm__ volatile("mrs %0, sctlr_el1" : "=r"(sctlr));
    uart_putb("sctlr_el1: ", sctlr);

    sctlr |= (1u<<0) | (1u<<2) | (1u<<12);
    __asm__ volatile("msr sctlr_el1, %0\nisb" :: "r"(sctlr) : "memory");

    uart_putb("sctlr_el1: ", sctlr);
}

