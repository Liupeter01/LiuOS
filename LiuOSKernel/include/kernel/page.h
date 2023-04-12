#ifndef _PAGE_H_
#define _PAGE_H_
#pragma once 
#include <armv8-a/memlayout.h>
#include <armv8-a/sysregs.h>

#define CONFIG_PGTABLE_LEVELS 3
#define CONFIG_ARM64_PA_BITS 39
#define ARM64_HW_PGTABLE_LEVELS(va_bits) (((va_bits) - 4) / (PAGE_SHIFT - 3))

//#define SWAPPER_PGTABLE_LEVELS  (CONFIG_PGTABLE_LEVELS - 1)                     //2M BLOCK块地址
//#define SWAPPER_DIR_SIZE     (SWAPPER_PGTABLE_LEVELS * PAGE_SIZE)
#define SWAPPER_DIR_SIZE     (3 * PAGE_SIZE)

#define PHYS_MASK_SHIFT		(CONFIG_ARM64_PA_BITS)
//#define IDMAP_PGTABLE_LEVELS	(ARM64_HW_PGTABLE_LEVELS(PHYS_MASK_SHIFT) - 1)  //2M BLOCK块地址
//#define IDMAP_DIR_SIZE       (IDMAP_PGTABLE_LEVELS * PAGE_SIZE)
#define SWAPPER_DIR_SIZE     (3 * PAGE_SIZE)

/*
 * Level 1 descriptor (PGD)
 */
#define PGDIR_SHIFT 30

/*
 * Level 2 descriptor (PMD).
 */
#define PMD_TYPE_MASK		(3 << 0)
#define PMD_TYPE_FAULT		(0 << 0)
#define PMD_TYPE_TABLE		(3 << 0)
#define PMD_TYPE_SECT		(1 << 0)
#define PMD_TABLE_BIT		(1 << 1)

/*
 * Level 3 descriptor (PTE).
 */
#define PTE_VALID 0b01  // level 0,1,2 descriptor: valid
#define PTE_TABLE 0b10  // level 0,1,2 descriptor: table
#define PTE_V     0b11  // level 3 descriptor: valid
// PTE_AF(Access Flag)
// 0 -- this block entry has not yet.
// 1 -- this block entry has been used.
#define PTE_AF  (1 << 10)

// PTE_AP(Access Permission) is 2bit field.
//        EL0   EL1
// 00 --   x    RW
// 01 --  RW    RW
// 10 --   x    RO
// 11 --  RO    RO
#define PTE_AP(ap)  (((ap) & 3) << 6)
#define PTE_nG  (1 << 11)
#define PTE_U   PTE_AP(1)
#define PTE_RO  PTE_AP(2)
#define PTE_URO PTE_AP(3)
#define PTE_PXN (1UL << 53)   // Privileged eXecute Never
#define PTE_UXN (1UL << 54)   // Unprivileged(user) eXecute Never
#define PTE_XN (PTE_PXN|PTE_UXN)  // eXecute Never
#define PTE_USER  (PTE_U|PTE_nG)

// Shareable attribute
#define PTE_SH(sh)    (((sh) & 3) << 8)
#define PTE_SH_OUTER  PTE_SH(2)   // outer sharable
#define PTE_SH_INNER  PTE_SH(3)   // inner sharable

#define PTE_INDX(i)   (((i) & 7) << 2)
#define PTE_DEVICE    (PTE_INDX(AI_DEVICE_nGnRnE_IDX) | PTE_SH_INNER)
#define PTE_NORMAL_NC (PTE_INDX(AI_NORMAL_NC_IDX) | PTE_SH_INNER)
#define PTE_NORMAL    (PTE_INDX(AI_NORMAL_IDX) | PTE_SH_INNER)

// shift a physical address to the right place for a PTE.
#define PA2PTE(pa)  ((uint64)(pa) & 0xfffffffff000)
#define PTE2PA(pte) ((uint64)(pte) & 0xfffffffff000)
#define PTE_FLAGS(pte)  ((pte) & (0x600000000003ff))

/*
 * Section
 */
#define PMD_SECT_VALID		(1 << 0)
#define PMD_SECT_USER		(1 << 6)		/* AP[1] */
#define PMD_SECT_RDONLY		(1<< 7)		/* AP[2] */
#define PMD_SECT_S		(3 << 8)
#define PMD_SECT_AF		(1 << 10)
#define PMD_SECT_NG		(1 << 11)
#define PMD_SECT_CONT		(1 << 52)
#define PMD_SECT_PXN		(1 << 53)
#define PMD_SECT_UXN		(1 << 54)
#endif //_PAGE_H_