/**
 * \defgroup peripherals peripherals
 * Raspberry Pi Memory layout file
 * @{
 * @file memorylayout.h
 * @brief Definition of user,kernel,MMIO registers addresses.
 */
#ifndef _MEMLAYOUT_H_
#define _MEMLAYOUT_H_
#pragma once
#include <armv8-a/basetype.h>

// First kernel virtual ram address 
// V:0xFFFF_FF80_0008_0000 ==> P:0x80000  (PHY_START)
#define KERNBASE  0xffffff8000000000	
#define PHY_START 0x80000
#define PHYSTOP (128*1024*1024)

// we first map 2MB low memory containing kernel code.
#define INIT_KERN_SZ	0x200000
#define INIT_KERNMAP 	(INIT_KERN_SZ + PHY_START)

/**
 * @details `PAGE_SIZE:	4KB`
 * - For kernel at address `0x80000`: 3584KB available for kernel till 4MB.
 */
#define PAGE_SHIFT	12				    /**< Bits to shift to define Page size. */
#define TABLE_SHIFT 	9				/**< Bits to shift to define Table size. */
#define SECTION_SHIFT	(PAGE_SHIFT + TABLE_SHIFT)	/**< Bits to shift to define Section size (= 21). */

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

#ifndef __ASSEMBLER__
static inline uint64 v2p(void *a) { return ((uint64) (a))  - (uint64)KERNBASE; }
static inline void *p2v(uint64 a) { return (void *) ((a) + (uint64)KERNBASE); }

/**Uart Base Address, we consider about vitual mmio mapping problem*/
extern volatile uchar *kernel_gpio_base;
/** * gic Base Address, we consider about vitual mmio mapping problem*/
extern volatile uchar *kernel_gic_base;
/*** aux Base Address, we consider about vitual mmio mapping problem*/
extern volatile uchar *kernel_aux_base;
/*** irq Base Address, we consider about vitual mmio mapping problem*/
extern volatile uchar *kernel_irq_base;
#endif

#define V2P(a) (((uint64) (a)) - (uint64)KERNBASE)
#define P2V(a) (((void *) (a)) + (uint64)KERNBASE)

#define V2P_WO(x) ((uint64)(x) - (uint64)KERNBASE)    // same as V2P, but without casts
#define P2V_WO(x) ((x) + (uint64)KERNBASE)    // same as V2P, but without casts

/**
 * 
 * \ingroup peripherals
 * The MMIO area base address
 * GPIO physical address (PA)
 * gic physical address (PA) TIMER_BASE = 0xFE003000
 * AUX physical address (PA)
 * IRQ physical address (PA)
 */
#define MMIO_KERNEL_BASE 0xffffffff00000000	
#define MMIO_BASE  0xFE000000
#define MMIO_END  0x100000000

#define GPIO_BASE (MMIO_BASE + 0x200000)
#define GIC_BASE  (MMIO_BASE + 0x15000)
#define AUX_BASE  0x15000
#define IRQ_BASE 0xB000
/** @} */

#endif //_MEMLAYOUT_H_
