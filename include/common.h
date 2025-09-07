// common.h
#ifndef __COMMON_H__
#define __COMMON_H__

// TODO: ---- Add printk -----//

// ---------- sysreg helpers -----------
#define READ_SYSREG64(name) ({ \
        uint64_t _v; \
        __asm__ volatile ("mrs %0, " #name : "=r"(_v)); \
        _v; \
})

#define WRITE_SYSREG64(name, val) do { \
    unint64_t _v = (uint64_t)(val); \
    __asm__ volatile ("msr " #name ", %0" :: "r"(_v) : "memory"); \
} while (0)


static inline void isb(void) { __asm__ volatile("isb" ::: "memory"); }
static inline void dsb_ish(void) { __asm__ volatile("dsb ish" ::: "memory"); }

#endif __COMMON_H__
