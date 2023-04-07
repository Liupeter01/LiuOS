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
// V:0xFFFF_FFFF_0008_0000 ==> P:0x80000  (PHY_START)
#define KERNBASE  0xFFFFFFFF00000000	
#define PHY_START 0x80000
#define GET_KERNE_LIMG_SIZE(end) (((uint64) (end)) - (uint64)PHY_START)
#define PHYS_STOP (PHY_START + 128 *1024 *1024)

// we first map 2MB low memory containing kernel code.
#define INIT_KERN_SZ	0x200000
#define INIT_KERNMAP 	(INIT_KERN_SZ + PHY_START)

#ifndef __ASSEMBLER__
static inline uint64 v2p(void *a) { return ((uint64) (a))  - (uint64)KERNBASE; }
static inline void *p2v(uint64 a) { return (void *) ((a) + (uint64)KERNBASE); }

/**
 * Uart Base Address, we consider about vitual mmio mapping problem
 */
extern volatile uchar *kernel_gpio_base;

/**
 * gic Base Address, we consider about vitual mmio mapping problem
 */
extern volatile uchar *kernel_gic_base;

/**
 * aux Base Address, we consider about vitual mmio mapping problem
 */
extern volatile uchar *kernel_aux_base;
#endif

#define V2P(a) (((uint64) (a)) - (uint64)KERNBASE)
#define P2V(a) (((void *) (a)) + (uint64)KERNBASE)

#define V2P_WO(x) ((uint64)(x) - (uint64)KERNBASE)    // same as V2P, but without casts
#define P2V_WO(x) ((x) + (uint64)KERNBASE)    // same as V2P, but without casts

/**
 * The MMIO area base address
 */
#define MMIO_BASE  0xFE000000
#define MMIO_END  0x100000000L

/**
 * \ingroup peripherals
 * GPIO physical address (PA)
 */
#define GPIO_BASE (MMIO_BASE + 0x200000)

/**
 * \ingroup peripherals
 * gic physical address (PA)
 * For raspi 4: TIMER_BASE = 0xFE003000
 */
#define GIC_BASE  (MMIO_BASE + 0x15000)

/**
 * \ingroup peripherals
 * AUX physical address (PA)
 */
#define AUX_BASE  0x15000
/** @} */

#endif //_MEMLAYOUT_H_
