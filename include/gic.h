/* SPDX-License-Identifier: MIT
 * Interrupt yourself
 */

#ifndef __HEADER_GIC_H__
#define __HEADER_GIC_H__

#include "common.h"

#ifdef PLATFORM_qemu
#define D_BASE 0x08000000
#define RD_BASE 0x080A0000
#else
#define D_BASE 0xfe600000
#define RD_BASE 0xfe680000
#endif

#define SGI_BASE (RD_BASE + 0x10000)

#define GICD_CTLR (D_BASE + 0x0000)
#define GICD_TYPER (D_BASE + 0x0004)

#define GICD_IGROUPRn (D_BASE + 0x0080)
#define GICD_IPRIORITYRn (D_BASE + 0x0400)
#define GICD_ISENABLERn (D_BASE + 0x0100)
#define GICD_ICENABLERn (D_BASE + 0x0180)
#define GICD_ISPENDRn (D_BASE + 0x0200)
#define GICD_ICPENDRn (D_BASE + 0x0280)
#define GICD_ISACTIVERn (D_BASE + 0x0300)
#define GICD_ICACTIVERn (D_BASE + 0x0380)
#define GICD_ICFGRn (D_BASE + 0x0C00)
#define GICD_IROUTERn (D_BASE + 0x6100)
#define GICD_IGRPMODRn (D_BASE + 0x0D00)

#define GICR_CTLR (RD_BASE + 0x0000)
#define GICR_TYPER (RD_BASE + 0x0008)
#define GICR_WAKER (RD_BASE + 0x0014)

#define GICR_IGROUP0 (SGI_BASE + 0x0080)
#define GICR_IPRIORITYRn (SGI_BASE + 0x0400)
#define GICR_ISENABLER0 (SGI_BASE + 0x0100)
#define GICR_ICENABLER0 (SGI_BASE + 0x0180)
#define GICR_ISPENDR0 (SGI_BASE + 0x0200)
#define GICR_ICPENDR0 (SGI_BASE + 0x0280)
#define GICR_ISACTIVER0 (SGI_BASE + 0x0300)
#define GICR_ICACTIVER0 (SGI_BASE + 0x0380)
#define GICR_ICFGR0 (SGI_BASE + 0x0C00)
#define GICR_ICFGR1 (SGI_BASE + 0x0C04)
#define GICR_NSACR (SGI_BASE + 0x0E00)
#define GICR_IGRPMODR0 (SGI_BASE + 0x0D00)

#define GICD_IROUTER_MODE_BIT 31

#define GICC_CTLR
#define GICC_PMR
#define GICC_BPR
#define GICC_IAR
#define GICC_EOIR
#define GICC_RPR
#define GICC_HPPIR
#define GICC_DIR

void gic_enable_intid(u32 intid);
void gic_disable_intid(u32 intid);
void gic_isactive_intid(u32 intid);
void gic_icactive_intid(u32 intid);
void gic_ispend_intid(u32 intid);
void gic_icpend_intid(u32 intid);
void gic_set_priority(u32 intid, u8 priority);
void gic_configure_trigger(u32 intid, bool edge);
void gic_set_group(u32 intid, bool grp);
void gic_set_groupmod(u32 intid, bool nonsecure);
void gicd_set_route(u32 intid, u64 mpidr);

static inline u64 gic_ack() { return READ_SYSREG64(ICC_IAR1_EL1); }
static inline void gic_eoi1(u64 intid) { WRITE_SYSREG64(ICC_EOIR1_EL1, intid); }
static inline void gic_dir(u64 intid) { WRITE_SYSREG64(ICC_DIR_EL1, intid); }

void gic_init();
void gicr_dump_info();
#endif // __HEADER_GIC_H__
