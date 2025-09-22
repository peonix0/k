/* SPDX-License-Identifier: MIT
 * May your timers never drift
 
 * I'm not the one!
*/

#include "generic_timer.h"

void timer_reg_write(timer_reg reg, u64 val) {
  switch (reg) {
  case CNTP_CTL_EL0:
    WRITE_SYSREG64(CNTP_CTL_EL0, val);
    break;
  case CNTP_CVAL_EL0:
    WRITE_SYSREG64(CNTP_CVAL_EL0, val);
    break;
  case CNTP_TVAL_EL0:
    WRITE_SYSREG64(CNTP_TVAL_EL0, val);
    break;

  case CNTV_CTL_EL0:
    WRITE_SYSREG64(CNTV_CTL_EL0, val);
    break;
  case CNTV_CVAL_EL0:
    WRITE_SYSREG64(CNTV_CVAL_EL0, val);
    break;
  case CNTV_TVAL_EL0:
    WRITE_SYSREG64(CNTV_TVAL_EL0, val);
    break;

  default:
    break;
  }
}

void timer_enable_el0_access() {
  // 0: EL0 Acess to PCT, VCT
  u64 regv = READ_SYSREG64(CNTKCTL_EL1);
  regv = regv | (0b11);
  WRITE_SYSREG64(CNTKCTL_EL1, regv);
}

void timer_interrupt_set(bool enable) {
  // 0: TIMER_ENABLE, IMASK_, ISTATUS
  WRITE_SYSREG64(CNTP_CTL_EL0, enable);
}

void timer_event_stream_init() {
  // 2: Enable, 3: Dir
  // [4 - 7] EVNTI, rate of events
  u64 regv = READ_SYSREG64(CNTKCTL_EL1);
  regv = regv | (0b111111 << 2);
  WRITE_SYSREG64(CNTKCTL_EL1, regv);
}

