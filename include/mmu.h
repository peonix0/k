// mmu.h
#ifndef __MMU_H__
#define __MMU_H__

#define MAIR_ATTR_DEVnGnRE  0x00
#define MAIR_ATTR_NORMALWB  0xff

#define L0_IDX(va) (((va) >> 39) & 0x1FF)
#define L1_IDX(va) (((va) >> 30) & 0x1FF)
#define L2_IDX(va) (((va) >> 21) & 0x1FF)

void mmu_enable(void);

#endif // __MMU_H__

