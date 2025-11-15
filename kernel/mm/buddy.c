/* SPDX-License-Identifier: MIT
 * I'll just hate it
*/

#include "mm/buddy.h"
#include "common.h"
#include "uart.h"

#undef DEBUG
#ifdef DEBUG
#define DBGSX(s, x) uart_logx(s, x)
#else
#define DBGSX(s, x)
#endif

#define MAX_ORDER 10
#define PG_BUDDY (1ull << 0)
#define PG_RESERVED (1ull << 1)

struct free_area {
  struct list_head freelist;
  u64 nr_free;
};

struct free_area free_area[MAX_ORDER];

static void __check_buddy_init() {
  u32 page_cnt = 0;
  for (u8 i = 0; i <= MAX_ORDER; i++) {
    page_cnt += (free_area[i].nr_free * (1ull << i));
  }

  if (page_cnt != TOTAL_UNRESERVED_PAGES) {
    pr_err("[mm/buddy]: Something wrong happening while buddy_init()!\n");
  }
}

void buddy_init() {
  i8 order = MAX_ORDER;
  u64 pfn = 0;
  while (order >= 0) {
    struct list_head *head, *prev;

    free_area[order].nr_free = 0;
    head = &free_area[order].freelist;
    head->prev = head->next = head;

    prev = head;

    while (pfn + (1ULL << order) <= TOTAL_UNRESERVED_PAGES) {
      page_t *pagehead;

      pagehead = &mem_map[pfn];

      pagehead->private = order;
      pagehead->flags |= (PG_BUDDY);

      prev->next = &pagehead->buddy_list;
      pagehead->buddy_list.prev = prev;
      pagehead->buddy_list.next = head;
      head->prev = &pagehead->buddy_list;

      prev = &pagehead->buddy_list;

      free_area[order].nr_free++;
      pfn += (1ull << order);
    }
    order--;
  }

  __check_buddy_init();
}

page_t *buddy_page_alloc(u8 order) {
  struct list_head *node, *head;
  page_t *page =  NULL, *rpage =  NULL;
  u32 pfn;

  if (order > MAX_ORDER)
    return NULL;

  DBGSX("[CHK1] order:", order);
  DBGSX("[CHK1] nr_free:", free_area[order].nr_free);

  if (!free_area[order].nr_free) {
    page = buddy_page_alloc(order + 1);

    DBGSX("[CHK2]:", (u64)page);
    if (!page) {

      DBGSX("[CHK2']:", (u64)page);
      return NULL;
    }

    pfn = __page_to_pfn(page);

    // Add right half to freelist[order]
    rpage = __pfn_to_page(pfn + (1u << order));
    head = &free_area[order].freelist;

    rpage->buddy_list.next = head->next;
    head->next = &rpage->buddy_list;
    rpage->buddy_list.prev = head;
    rpage->buddy_list.next->prev = &rpage->buddy_list;

    rpage->refcount = 0;
    rpage->private = order;
    rpage->flags |= PG_BUDDY;
    free_area[order].nr_free++;

    // return left half
    page->flags &= ~(PG_BUDDY);
    page->private = order;

    DBGSX("[CHK5 L]:", (u64)page);
    DBGSX("[CHK5 R]:", (u64)rpage);
    return page;
  }

  node = free_area[order].freelist.next;

  free_area[order].freelist.next = node->next;
  node->next->prev = &free_area[order].freelist;

  free_area[order].nr_free--;

  node->next = NULL;
  node->prev = NULL;

  page = container_of(node, page_t, buddy_list);
  page->private = order;
  page->flags &= ~(PG_BUDDY);

   DBGSX("[CHK6]:", (u64)page);
  return page;
}


void buddy_free_page(page_t *free_page) {
  u32 pfn;
  struct list_head *freelist;
  u16 order;
  page_t *page;

  DBGSX("page addr: ", (u64)free_page);
  page = free_page;
  order = page->private;

  if (order > MAX_ORDER) {
    pr_err("[buddy_free]: Can't free Memory, fuck you!\n");
    return;
  }

  if (page->flags & PG_BUDDY) {
    pr_err("[buddy_free]: Double free?");
    return;
  }

  pfn = __page_to_pfn(page);

  while (order < MAX_ORDER) {
    page_t *buddy_page;
    u32 buddy_pfn;

    if (!free_area[order].nr_free)
      break;

    buddy_pfn = pfn ^ (1u << order);
    buddy_page = __pfn_to_page(buddy_pfn);
    if (buddy_page->refcount || !(buddy_page->flags & (PG_BUDDY)) ||
        buddy_page->private != order) {
      break;
    }

    buddy_page->flags &= ~(PG_BUDDY);
    buddy_page->private = 0;

    buddy_page->buddy_list.prev->next = buddy_page->buddy_list.next;
    buddy_page->buddy_list.next->prev = buddy_page->buddy_list.prev;
    free_area[order].nr_free--;

    if (buddy_pfn < pfn) {
      page = buddy_page;
      pfn = buddy_pfn;
    }
    order++;
  }

  freelist = &free_area[order].freelist;

  page->buddy_list.next = freelist->next;
  page->buddy_list.prev = freelist;
  freelist->next = &page->buddy_list;
  page->buddy_list.next->prev = &page->buddy_list;

  page->private = order;
  page->flags |= PG_BUDDY;
  page->refcount = 0;

  free_area[order].nr_free++;
}

void buddy_mm_stats() {
  u64 free_pages = 0;
  for(u8 i=0; i <= MAX_ORDER; i++){
    free_pages += free_area[i].nr_free * (1ull << i);
  }

  pr_log("Total Available Pages: ", TOTAL_UNRESERVED_PAGES);
  pr_log("Free Pages: ", free_pages);
}
// void mm_reserve(u64 pa, u64 len) {}
