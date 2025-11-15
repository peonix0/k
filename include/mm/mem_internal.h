/* SPDX-License-Identifier: MIT
 * I hate it
*/

#ifndef __HEADER_MEM_H__
#define __HEADER_MEM_H__

#include "common.h"

#define PAGE_SIZE 4096

struct page {
  i64 refcount;

  /*
   * PG_BUDDY: 0bit
   * PG_RESERVED: 1bit
   */
  u8 flags;

  union {
    struct list_head lru;
    struct list_head buddy_list;
  };

  /* Indicates order in the buddy system if PageBuddy */
  u64 private;
};

typedef struct page page_t;

/* I can do whatever i wish */
/* Let's put our pages metadata over 0x40800000 */

static page_t *mem_map = (page_t *)0x40800000;
static inline u32 __page_to_pfn(page_t *page) { return (u32)(page - mem_map); }
static inline page_t * __pfn_to_page(u32 pfn) { return &mem_map[pfn]; }

/* assuming, or have it as 128MB physical mem, so entries are ~2^15 */
/* Let's keep 2MB for this, will start allocating mem from 0x40A00000 */
#define UNRESERVED_MEM_BASE  0x40A00000
#define MEM_BASE  0x40000000

#define TOTAL_UNRESERVED_PAGES  (0x8000000 - 0xA00000) / PAGE_SIZE

#endif // __HEADER_MEM_H__
