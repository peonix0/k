/* SPDX-License-Identifier: MIT
 * One to rule them all
*/

#ifndef __HEADER_COMMON_H__
#define __HEADER_COMMON_H__

typedef __INT8_TYPE__     i8;
typedef __UINT8_TYPE__    u8;
typedef __INT16_TYPE__    i16;
typedef __UINT16_TYPE__   u16;
typedef __INT32_TYPE__    i32;
typedef __UINT32_TYPE__   u32;
typedef __INT64_TYPE__    i64;
typedef __UINT64_TYPE__   u64;

typedef __INTPTR_TYPE__   iptr;
typedef __UINTPTR_TYPE__  uptr;
typedef __PTRDIFF_TYPE__  isize;
typedef __SIZE_TYPE__     usize;

typedef _Bool             bool;
#define true  ((bool)1)
#define false ((bool)0)


// TODO: ---- Add printk -----//

// ---------- sysreg helpers -----------
#define READ_SYSREG64(name) ({ \
        u64 _v; \
        __asm__ volatile ("mrs %0, " #name : "=r"(_v)); \
        _v; \
})

#define WRITE_SYSREG64(name, val) do { \
    u64 _v = (u64)(val); \
    __asm__ volatile ("msr " #name ", %0" :: "r"(_v) : "memory"); \
} while (0)


static inline void isb(void) { __asm__ volatile("isb" ::: "memory"); }
static inline void dsb_ish(void) { __asm__ volatile("dsb ish" ::: "memory"); }

#endif // __HEADER_COMMON_H__
