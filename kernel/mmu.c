#include "mmu.h"
#include "uart.h"

// __attribute__((aligned(4096))) static u64 L0[512];
__attribute__((aligned(4096))) static u64 L1[512]; // 1GB Each
//__attribute__((aligned(4096))) static u64 L2_kernel[512]; // 2 MB Each
__attribute__((aligned(4096))) static u64 L2_t1[512];


// --------------------- DUMPING- JUMPING-----------------------------------//
// static void dump_sctlr(u64 sctlr) {
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


static inline u64 desc_block_2m(u64 pa, int attr_idx) {
    u64 d = (pa & ~((1ULL<<21)-1))      // 2MiB aligned output address
               | (1ULL<<10)                  // AF
               | ((u64)attr_idx << 2)   // AttrIndx[4:2]
               | (1 ? (1ULL<<54) : 0)       // UXN
               | (0 ? (1ULL<<53) : 0)       // PXN
               | (1ULL<<0);                  // VALID, TYPE=BLOCK
    // AP bits default 00 (EL1 RW), SH=Inner (3<<8)
    d |= (3ULL<<8);
    return d;
}

static inline u64 desc_block_1g(u64 pa_page, int attr_idx) {
    u64 d = (pa_page & ~((1ULL<<30)-1))
                | (1ULL<<10)
                | ((u64)attr_idx << 2) // AtrrIndx[4:2]
                | (1 ? (1ULL<<54) : 0) // UXN
                | (0 ? (1ULL<<53) : 0) // PXN
                | (1ULL << 0);  // VALID, TYPE=Table Entry

    d |= (3ULL << 8);
    return d;
}

static inline u64 desc_page_4k(u64 pa_page, int attr_idx) {
    u64 e = (pa_page & ~((1ULL<<12)-1))
                | (1ULL<<10)
                | ((u64)attr_idx << 2) // AtrrIndx[4:2]
                | (1 ? (1ULL<<54) : 0) // UXN
                | ( 0 ? (1ULL<<53) : 0) // PXN
                | (3ULL << 0);  // VALID, TYPE=Table Entry

    e |= (3ULL << 8);
    return e;
}

// point to a 4KB page
static inline u64 desc_table(u64 pa) {
    return (pa & ~0xFFFULL) | (1ULL<<1) | 1ULL; // VALID, TYPE=TABLE
}

static inline u64 tcr_value(void){
    const u64 T0SZ  = 32ULL;   // 32-bit VA
    const u64 TG0   = 0ULL;    // 4KB
    const u64 SH0   = 3ULL;    // Inner
    const u64 ORGN0 = 1ULL<<10 | 1ULL<<11; // WB WA
    const u64 IRGN0 = 1ULL<<8  | 1ULL<<9;  // WB WA
    const u64 IPS   = 0b010ULL<<32;        // 48-bit PA
    return (T0SZ) | (TG0<<14) | (SH0<<12) | (IRGN0) | (ORGN0) | IPS;
}

extern char __text_start[]; // pick your kernel base symbol
void mmu_enable(void){
    // map kernel @ 0x40200000 as Normal WB

    u64 kva = 0x40200000ULL;
    L1[L1_IDX(kva)] = desc_block_1g(kva, 0x1);
    //L0[L0_IDX(kva)] = desc_table((u64) L1_kernel);

    // Map QEMU PL011 or RK UART as Device (AttrIdx0)
#ifdef PLATFORM_qemu
    u64 uva = 0x09000000ULL;
#else
    u64 uva = 0xFEB50000ULL;
#endif
    u64 l2_uart_entry = desc_block_2m(uva, 0x0);
    L2_t1[L2_IDX(uva)] = l2_uart_entry;


    // map GIC
    u64 gic_dva = 0x08000000;  //64 KB
    // u64 gic_rdva = 0x080A0000; // 2MB, I only care about single frame (128KB)

    L2_t1[L2_IDX(gic_dva)] = desc_block_2m(gic_dva, 0x0);

    // rdva region already mapped by above block entry (atleast one frame)
    //L2_t1[L2_IDX(gic_rdva)] = desc_block_2m(gic_rdva, 0x0);

    L1[L1_IDX(uva)] = desc_table((u64) L2_t1);

    // 2) Program MAIR/TCR/TTBR0
    u64 mair = (MAIR_ATTR_DEVnGnRE << 0) | (MAIR_ATTR_NORMALWB << 8);
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
    u64 sctlr;
    __asm__ volatile("mrs %0, sctlr_el1" : "=r"(sctlr));
    uart_logb("sctlr_el1: ", sctlr);

    sctlr |= (1u<<0) | (1u<<2) | (1u<<12);
    __asm__ volatile("msr sctlr_el1, %0\nisb" :: "r"(sctlr) : "memory");

    uart_logb("sctlr_el1: ", sctlr);
}

