/* SPDX-License-Identifier: MIT
 * Blinded or magical?
 */

#include "gic.h"
#include "uart.h"

// TODO: rewrite?
void gic_init() {
  uart_puts("k: gic init!\n");
  /* Enable System Register Interface */
  u64 reg = 0b1 | READ_SYSREG64(ICC_SRE_EL1);
  WRITE_SYSREG64(ICC_SRE_EL1, reg);
  isb();

  WRITE_SYSREG64(ICC_PMR_EL1, 0xFF);
  WRITE_SYSREG64(ICC_BPR0_EL1, 0b011);
  WRITE_SYSREG64(ICC_IGRPEN1_EL1, 0b1);

  // Setting up EOImode(bit1:0), both priority drop and interrupt deactivation
  reg = (~0b10) & READ_SYSREG64(ICC_CTLR_EL1);
  WRITE_SYSREG64(ICC_CTLR_EL1, reg);

  // Enable IGroupNS1
  reg = mmio_r32(GICD_CTLR);
  uart_putb(" GICD_CTLR: ", reg);
  mmio_w32(GICD_CTLR, reg | 0b10);

  // Mark CPU online
  reg = mmio_r32(GICR_WAKER);
  uart_putb(" GICD_WAKER: ", reg);
  mmio_w32(GICR_WAKER, reg & (~0b10));

  // TODO: move it to redistributor specific code
  // Specifies which group this interrupt belongs to GRP0: 0bit,...
  mmio_w32(GICR_IGROUP0, 0xFFFFFFFF);
  mmio_w32(GICR_IGRPMODR0, 0xFFFFFFFF);

  // maybe required during SMP
  // reg = mmio_r64(GICR_TYPER);
  // uart_putb(" GICR_TYPER: ", reg);

  // reg = READ_SYSREG64(MPIDR_EL1);
  // uart_putb(" MPIDR_EL1: ", reg);

  reg = READ_SYSREG64(ICC_RPR_EL1) & 0xFF;
  uart_puts(" ICC_RPR_EL1: ");
  uart_putx(reg);
  uart_puts("\n");

  reg = READ_SYSREG64(DAIF);
  uart_putb(" DAIF: ", reg);
  __asm__ __volatile__("msr daifclr, #7");
}

void gic_enable_intid(u32 intid) {
  u8 bitn = intid % 32;
  u64 gic_isenable_reg;

  if (intid > 31) {
    gic_isenable_reg = GICD_ISENABLERn + 4 * (intid / 32);
  } else {
    gic_isenable_reg = GICR_ISENABLER0;
  }

  mmio_w32(gic_isenable_reg, 1u << bitn);
}

void gic_disable_intid(u32 intid) {
  u8 bitn = intid % 32;
  u64 gic_icenable_reg;

  if (intid > 31) {
    gic_icenable_reg = GICD_ICENABLERn + 4 * (intid / 32);
  } else {
    gic_icenable_reg = GICR_ICENABLER0;
  }

  mmio_w32(gic_icenable_reg, 1u << bitn);
}

void gic_isactive_intid(u32 intid) {
  u8 bitn = intid % 32;
  u64 gic_isactive_reg;

  if (intid > 31) {
    gic_isactive_reg = GICD_ISACTIVERn + 4 * (intid / 32);
  } else {
    gic_isactive_reg = GICR_ISACTIVER0;
  }

  mmio_w32(gic_isactive_reg, 1u << bitn);
}

void gic_icactive_intid(u32 intid) {
  u8 bitn = intid % 32;
  u64 gic_icactive_reg;

  if (intid > 31) {
    gic_icactive_reg = GICD_ISACTIVERn + 4 * (intid / 32);
  } else {
    gic_icactive_reg = GICR_ICACTIVER0;
  }

  mmio_w32(gic_icactive_reg, 1u << bitn);
}

void gic_ispend_intid(u32 intid) {
  u8 bitn = intid % 32;
  u64 gic_ispend_reg;

  if (intid > 31) {
    gic_ispend_reg = GICD_ISPENDRn + 4 * (intid / 32);
  } else {
    // TODO:
    // gic_ispend_reg = GICR_ISPENDR0;
  }

  mmio_w32(gic_ispend_reg, 1u << bitn);
}

void gic_icpend_intid(u32 intid) {
  u8 bitn = intid % 32;
  u64 gic_icpend_reg;

  if (intid > 31) {
    gic_icpend_reg = GICD_ICPENDRn + 4 * (intid / 32);
  } else {
    gic_icpend_reg = GICR_ICPENDR0;
  }

  mmio_w32(gic_icpend_reg, 1u << bitn);
}

void gic_set_priority(u32 intid, u8 priority) {
  u8 byte_offset = intid % 4;
  u64 gic_priority_reg;

  if (intid > 32) {
    gic_priority_reg = GICD_IPRIORITYRn + 4 * (intid / 4);
  } else {
    gic_priority_reg = GICR_IPRIORITYRn + 4 * (intid / 4);
  }

  mmio_w8(gic_priority_reg + byte_offset, priority);
}

/* Set if using GRP1(1), or GRPS0(0) */
void gic_set_group(u32 intid, bool grp) {
  u8 bitoff = intid % 32;
  u64 gic_igroup_reg;

  if (intid < 31) {
    gic_igroup_reg = GICD_IGROUPRn + 4 * (intid / 32);
  } else {
    gic_igroup_reg = GICR_IGROUP0;
  }

  u32 reg = mmio_r32(gic_igroup_reg);
  mmio_w32(gic_igroup_reg, (reg & ~(1u << bitoff)) | (grp << bitoff));
}

/* Set if using GRP_NS1(1), or GRP_S1(0) */
void gic_set_groupmod(u32 intid, bool nonsecure) {
  u8 bitoff = intid % 32;
  u64 gic_igrpmod_reg;

  if (intid > 31) {
    gic_igrpmod_reg = GICD_IGRPMODRn + 4 * (intid / 32);
  } else {
    gic_igrpmod_reg = GICR_IGRPMODR0;
  }

  u32 reg = mmio_r32(gic_igrpmod_reg);
  mmio_w32(gic_igrpmod_reg, (reg & ~(1u << bitoff)) | nonsecure << bitoff);
}

void gic_configure_trigger(u32 intid, bool edge) {
  /* Only MSB matter
   * 00: level triggered
   * 10: Edge Triggered
   */

  u8 shift = (intid % 16) * 2 + 1;
  u64 gic_icfg_reg;

  if (intid < 16) {
    gic_icfg_reg = GICR_ICFGR0;
  } else if (intid < 32) {
    gic_icfg_reg = GICR_ICFGR1;
  } else {
    gic_icfg_reg = GICD_ICFGRn + 4 * (intid / 16);
  }

  u32 reg = mmio_r32(gic_icfg_reg);
  mmio_w32(gic_icfg_reg, (reg & ~(1u << shift)) | (edge << shift));
}

/* Set PE for intid */
void gicd_set_route(u32 intid, u64 mpidr) {
  u64 gic_iroute_reg = GICD_IROUTERn + 8 * (intid - 32);
  mmio_w64(gic_iroute_reg, mpidr);
}

void gicr_dump_info() {
  u32 reg = mmio_r32(GICR_ISENABLER0);
  uart_putb(" GICR_ISENABLER0: ", reg);

  reg = mmio_r32(GICR_ISACTIVER0);
  uart_putb(" GICR_ISACTIVER0: ", reg);

  reg = mmio_r32(GICR_ISPENDR0);
  uart_putb(" GICR_ISPEND0: ", reg);
}
