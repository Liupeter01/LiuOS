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
#include <stdint.h>
#include <armv8-a/basetype.h>
#include <armv8-a/memlayout.h>

// kernel page table, reserved in the kernel.ld
extern uint64 _kernel_pgtbl;
extern uint64 _user_pgtbl;
extern uint64 _K_l2_pgtbl;
extern uint64 _U_l2_pgtbl;

extern uint64 *kernel_pgtbl;
extern uint64 *user_pgtbl;

extern uint64 __end;

// load main funtion's address
extern void main(void);

#define PAGE_SHIFT	12				    /**< Bits to shift to define Page size. */
#define TABLE_SHIFT 	9				/**< Bits to shift to define Table size. */
#define SECTION_SHIFT	(PAGE_SHIFT + TABLE_SHIFT)	/**< Bits to shift to define Section size (= 21). */

/**
 * @details `PAGE_SIZE:	4KB`
 * - For kernel at address `0x0`: Start of stack, 4MB available for kernel (except stack).
 * - For kernel at address `0x8000`: 4064KB available for kernel till 4MB.
 * - For kernel at address `0x80000`: 3584KB available for kernel till 4MB.
 */
#define PAGE_SIZE	(1 << PAGE_SHIFT)		        /**< - `2^12 = 4096 = 4KB` */
#define SECTION_SIZE	(1 << SECTION_SHIFT)		/**< - `2^21 = 2048KB = 2MB` */

/** 4MB (First 4 MB of memory reserved for the kernel image and init task stack). */
#define LOW_MEMORY	(2 * SECTION_SIZE)		/**< - `2*2MB = 4MB` */
/** Start of Memory Mapped Registers. @see MMIO_BASE */
#define HIGH_MEMORY	MMIO_BASE

/** Available memory for paging */
#define PAGING_MEMORY	(HIGH_MEMORY - LOW_MEMORY)	/**< - `MMIO_BASE - 4MB` */
/** Available 4KB pages */
#define PAGING_PAGES	(PAGING_MEMORY / PAGE_SIZE)	/**< - `(MMIO_BASE - 4MB) / 4KB` */

// align_up/down: al must be of power of 2
#define align_up(sz, al) (((uint64)(sz)+ (uint64)(al)-1) & ~((uint64)(al)-1))
#define align_dn(sz, al) ((uint64)(sz) & ~((uint64)(al)-1))
//
// Since ARMv6, you may use two page tables, one for kernel pages (TTBR1),
// and one for user pages (TTBR0). We use this architecture. Memory address
// lower than UVIR_BITS^2 is translated by TTBR0, while higher memory is
// translated by TTBR1.
// Kernel pages are create statically during system initialization. It use
// 2MB page mapping. User pages use 4K pages.
//
#define PTE_VALID 1  // level 0,1,2 descriptor: valid
#define PTE_TABLE 2  // level 0,1,2 descriptor: table
#define PTE_V     3  // level 3 descriptor: valid
// PTE_AF(Access Flag)
//
// 0 -- this block entry has not yet.
// 1 -- this block entry has been used.
#define PTE_AF  (1 << 10)
#define PTE_nG  (1 << 11)
// PTE_AP(Access Permission) is 2bit field.
//        EL0   EL1
// 00 --   x    RW
// 01 --  RW    RW
// 10 --   x    RO
// 11 --  RO    RO
#define PTE_AP(ap)  (((ap) & 3) << 6)
#define PTE_U   PTE_AP(1)
#define PTE_RO  PTE_AP(2)
#define PTE_URO PTE_AP(3)
#define PTE_PXN (1UL << 53)   // Privileged eXecute Never
#define PTE_UXN (1UL << 54)   // Unprivileged(user) eXecute Never
#define PTE_XN (PTE_PXN|PTE_UXN)  // eXecute Never
#define PTE_USER  (PTE_U|PTE_nG)

#define BLOCK_ENTRY_PTE (PTE_AF | PTE_NORMAL | PTE_VALID)

// Shareable attribute
#define PTE_SH(sh)    (((sh) & 3) << 8)
#define PTE_SH_OUTER  PTE_SH(2)   // outer sharable
#define PTE_SH_INNER  PTE_SH(3)   // inner sharable

// attribute index
// index is set by mair_el1
#define AI_DEVICE_nGnRnE_IDX  0
#define AI_NORMAL_NC_IDX      1
#define AI_NORMAL_CACHE_IDX   2

// memory type
#define MT_DEVICE_nGnRnE  0x0
#define MT_NORMAL_NC      0x44
#define MT_NORMAL_CACHE   0xff

#define PTE_INDX(i)   (((i) & 7) << 2)
#define PTE_DEVICE    (PTE_INDX(AI_DEVICE_nGnRnE_IDX)|PTE_SH_INNER)
#define PTE_NORMAL_NC PTE_INDX(AI_NORMAL_NC_IDX)|PTE_SH_INNER
#define PTE_NORMAL    (PTE_INDX(AI_NORMAL_CACHE_IDX)|PTE_SH_INNER)

// shift a physical address to the right place for a PTE.
#define PA2PTE(pa)  ((uint64)(pa) & 0xfffffffff000)
#define PTE2PA(pte) ((uint64)(pte) & 0xfffffffff000)

#define PTE_FLAGS(pte)  ((pte) & (0x600000000003ff))

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

// access permission for page directory/page table entries.
//#define AP_NA       0x00    // no access
//#define AP_KO       0x01    // privilaged access, kernel: RW, user: no access
//#define AP_KUR      0x02    // no write access from user, read allowed
//#define AP_KU       0x03    // full access

// domain definition for page table entries
//#define DM_NA       0x00    // any access causing a domain fault
//#define DM_CLIENT   0x01    // any access checked against TLB (page table)
//#define DM_RESRVED  0x02    // reserved
//#define DM_MANAGER  0x03    // no access check

//#define PE_CACHE    (1 << 3)// cachable
//#define PE_BUF      (1 << 2)// bufferable

//#define PE_TYPES    0x03    // mask for page type
//#define KPDE_TYPE   0x02    // use "section" type for kernel page directory
//#define UPDE_TYPE   0x01    // use "coarse page table" for user page directory
//#define PTE_TYPE    0x02    // executable user page(subpage disable)

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

/**
 * Function for allocating memory pages.
 * @return
 * - On success: Starting address (64-bit) of the allocated page.
 * - On failure: 0
 */
uint64_t get_free_page();

/**
 * Function for deallocating memory pages.
 * @param p 64-bit address of page to be deallocated.
 */
void free_page(uint64_t p);
/** @} */

#endif //_MMU_H_