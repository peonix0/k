/* SPDX-License-Identifier: MIT
 * I'll just hate it
*/

#include "mm/buddy.h"
#include "mm/mem_internal.h"
#include "mm/mem.h"

void mm_init() {
  for (u32 i = 0; i < TOTAL_UNRESERVED_PAGES; i++) {
    mem_map[i].refcount = 0;
    mem_map[i].flags = 0b0;
    mem_map[i].private = 0;
  }

  buddy_init();
}

u64 page_to_pa(page_t *page) {
  u32 pfn;
  pfn = __page_to_pfn(page);

  return (UNRESERVED_MEM_BASE + PAGE_SIZE * pfn);
}

page_t *pa_to_page(u64 pa) {
  u32 pfn;

  if(pa < UNRESERVED_MEM_BASE)
    return NULL;

  pa &= ~((u64)PAGE_SIZE - 1);
  pfn = (pa - UNRESERVED_MEM_BASE) / PAGE_SIZE;
  if (pfn >= TOTAL_UNRESERVED_PAGES)
    return NULL;

  return &mem_map[pfn];
}

page_t *alloc_page(u8 order) {
  return buddy_page_alloc(order);
}

void free_page(page_t *page) {
  buddy_free_page(page);
}

void mm_stats() {
  buddy_mm_stats();
}
