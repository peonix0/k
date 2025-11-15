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

#define NULL 0


static inline void mmio_w8(u64 a, u8 v){ *(volatile u8*)a = v; }
static inline u32 mmio_r8(u64 a){ return *(volatile u8*)a; }


static inline void mmio_w32(u64 a, u32 v){ *(volatile u32*)a = v; }
static inline u32 mmio_r32(u64 a){ return *(volatile u32*)a; }

static inline void mmio_w64(u64 a, u64 v){ *(volatile u64*)a = v; }
static inline u32 mmio_r64(u64 a){ return *(volatile u64*)a; }


// TODO: ---- Add printk -----//


void debug_backtrace();

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

#define offsetof(type, member) ((isize) & (((type *)0)->member))

#define container_of(ptr, type, member)                                        \
  ((type *)((char *)(ptr) - offsetof(type, member)))

struct list_head {
  struct list_head *next, *prev;
};


#endif // __HEADER_COMMON_H__
