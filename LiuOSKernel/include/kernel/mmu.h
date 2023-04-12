/**
 * \defgroup memory memory
 * Code for managing  memory.
 * @{
 * @file mmu.h
 * @brief Definition of basic memory sizes and functions.
 */
#ifndef _MMU_H_
#define _MMU_H_
#pragma once 
#include <armv8-a/basetype.h>
#include <armv8-a/memlayout.h>

//
//  39bit(=512GB) Virtual Address
//
//     63   39 38    30 29    21 20    12 11       0
//    +-------+--------+--------+--------+----------+
//    | TTBRn | level1 | level2 | level3 | page off |
//    |       | (PUD)  | (PMD)  | (PTE)  |          |
//    +-------+--------+--------+--------+----------+
//

// extract the three 9-bit page table indices from a virtual address.
#define PXMASK          0x1FF // 9 bits
#define PXSHIFT(level)  (39-(level)*9)
#define PX(level, va) ((((uint64)(va)) >> PXSHIFT(level)) & PXMASK)

#define ENTRY_VALID	0x01
#define ENTRY_BLOCK	0x00
#define ENTRY_TABLE	0x02
#define ENTRY_PAGE	0x02
#define ENTRY_MASK	0x03
#define ENTRY_FALUT	0x00

#define MEM_ATTR_IDX_0	(0 << 2)
#define MEM_ATTR_IDX_1	(1 << 2)
#define MEM_ATTR_IDX_2	(2 << 2)
#define MEM_ATTR_IDX_3	(3 << 2)
#define MEM_ATTR_IDX_4	(4 << 2)
#define MEM_ATTR_IDX_5	(5 << 2)
#define MEM_ATTR_IDX_6	(6 << 2)
#define MEM_ATTR_IDX_7	(7 << 2)

#define NON_SECURE_PA (1 << 5)

#define AP_RW_1     (0 << 6)
#define AP_RW_1_0   (1 << 6)
#define AP_RO_1     (2 << 6)
#define AP_RO_1_0   (3 << 6)
#define AP_MASK     (3 << 6)

#define SH_NON_SH   (0 << 8)
#define SH_UNPRED   (1 << 8)
#define SH_OUT_SH   (2 << 8)
#define SH_IN_SH    (3 << 8)

#define ACCESS_FLAG (1 << 10)

#define NORMAL_MEMORY_BLOCK_MAPPING ( \
    ACCESS_FLAG | \
    SH_IN_SH | \
    AP_RW_1 | \
    NON_SECURE_PA | \
    MEM_ATTR_IDX_4 | \
    ENTRY_BLOCK | \
    ENTRY_VALID | \
    UXN \
)

#define DEVICE_MEMORY_BLOCK_MAPPING ( \
    ACCESS_FLAG | \
    AP_RW_1 | \
    MEM_ATTR_IDX_0 | \
    ENTRY_BLOCK | \
    ENTRY_VALID \
)

#define PXN         (0x20000000000000)
#define UXN         (0x40000000000000)

#define PG_ADDR_MASK	0xFFFFFFFFF000	// bit 47 - bit 12

// 1st level 
#define VA_BITS 39
#define PGD_SHIFT	30
#define PGD_SZ		(1 << PGD_SHIFT)
#define PGD_MASK 	(~(PGD_SZ - 1))			// offset for page directory entries
#define PTRS_PER_PGD	(1 << (VA_BITS - PGD_SHIFT))  
#define PGD_IDX(v)	(((uint64)(v) >> PGD_SHIFT) & (PTRS_PER_PGD - 1))	// index for page table entry

// 2nd-level (2MB) page directory (always maps 1MB memory)
#define PMD_SHIFT	21					// shift how many bits to get PDE index
#define PMD_SZ      	(1 << PMD_SHIFT)
#define PMD_MASK	(~(PMD_SZ - 1))				// offset for page directory entries
#define PTRS_PER_PMD	512
#define PMD_IDX(v)	(((uint64)(v) >> PMD_SHIFT) & (PTRS_PER_PMD - 1))	// index for page table entry

// 3rd-level (4k) page table
#define PTE_SHIFT	12					// shift how many bits to get PTE index
#define PTE_SZ		(1 << PTE_SHIFT)
#define PTRS_PER_PTE	512
#define PTE_ADDR(v)	align_dn (v, PTE_SZ)
#define PTE_IDX(v)	(((uint64)(v) >> PTE_SHIFT) & (PTRS_PER_PTE - 1))

// size of two-level page tables
#define UADDR_BITS	28					// maximum user-application memory, 256MB
#define UADDR_SZ	(1 << UADDR_BITS)			// maximum user address space size

// must have NUM_UPDE == NUM_PTE
//#define NUM_UPDE	(1 << (UADDR_BITS - PMD_SHIFT))		// # of PDE for user space
//#define NUM_PTE	(1 << (PMD_SHIFT - PTE_SHIFT))		// how many PTE in a PT

#define PT_SZ		(PTRS_PER_PTE << 3)			// user page table size (1K)
#define PT_ADDR(v)	align_dn(v, PT_SZ)			// physical address of the PT
#define PT_ORDER	10

#ifndef __ASSEMBLER__
/**
 * Function for allocating memory pages.
 * @return
 * - On success: Starting address (64-bit) of the allocated page.
 * - On failure: 0
 */
unsigned long long get_free_page();

/**
 * Function for deallocating memory pages.
 * @param p 64-bit address of page to be deallocated.
 */
void free_page(uint64_t p);
#endif
/** @} */

#endif //_MMU_H_