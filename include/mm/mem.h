/* SPDX-License-Identifier: MIT
 * I'll just hate it
 */

#include "common.h"

struct page;
typedef struct page page_t;

void mm_init();

u64 page_to_pa(page_t *page);
page_t *pa_to_page(u64 pa);

page_t *alloc_page(u8 order);
void free_page(page_t *page);
void mm_stats();

// void mm_reserve(u64 pa, u64 len);
