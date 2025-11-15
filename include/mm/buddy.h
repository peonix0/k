/* SPDX-License-Identifier: MIT
 * I still hate it
*/

#include "common.h"
#include "mm/mem_internal.h"

#define pr_err(x) uart_logs(x)
#define pr_log(s, d) uart_logx(s, d);

void buddy_init();
page_t *buddy_page_alloc(u8 order);
void buddy_free_page(page_t *free_page);
void buddy_mm_stats();




