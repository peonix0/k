/* SPDX-License-Identifier: MIT
 * It's all in my head

 * who let you write?
*/
#ifndef __HEADER_GENERIC_TIMER_H__
#define __HEADER_GENERIC_TIMER_H__

#include "common.h"

#define INTID_GENERIC_TIMER_P 30
#define INTID_GENERIC_TIMER_V 27

typedef enum {
  CNTP_CTL_EL0, // 0: Enable, IMask, IStatus
  CNTP_CVAL_EL0,
  CNTP_TVAL_EL0, // 32bit

  CNTV_CTL_EL0,
  CNTV_CVAL_EL0,
  CNTV_TVAL_EL0,

  CNTKCTL_EL1, // User acces reg
} timer_reg;

static inline void timer_setp_tval(u32 val) {
  WRITE_SYSREG64(CNTP_TVAL_EL0, val);
}
static inline void timer_setv_tval(u32 val) {
  WRITE_SYSREG64(CNTV_TVAL_EL0, val);
}
static inline u64 timer_get_vct() { return READ_SYSREG64(CNTVCT_EL0); }
static inline u64 timer_get_pct() { return READ_SYSREG64(CNTPCT_EL0); }
static inline u32 timer_get_freq() { return READ_SYSREG64(CNTFRQ_EL0); }

static inline bool timer_istatus() { return (1 << 2) & READ_SYSREG64(CNTP_CTL_EL0); }

void timer_reg_write(timer_reg reg, u64 val);
void timer_interrupt_set(bool enable);
void timer_enable_el0_access();
void timer_event_stream_init();

// TODO: Scaling: seems can only do in EL3

#endif /* __HEADER_GENERIC_TIMER_H__ */
