/**
 * \ingroup entry8
 * @file sysregs.h
 * @brief Definition of armv8-a system registers
 * @details Needed for Exception level change.
 */

#ifndef SYSREGS_H
#define SYSREGS_H

/**
 * \ingroup entry8
 * \name SCTLR_EL1
 * @{
 * SCTLR_EL1, System Control Register (EL1) \n
 * Section D13.2.116 of AArch64-Reference-Manual.
 * 此部分用于将EL2切换到EL1进行使用
 */

/** \ingroup entry8 Reserved bits are initialized to 1. */
#define SCTLR_RESERVED		(3 << 28) | (3 << 22) | (1 << 20) | (1 << 11)
/** Work only with little-endian format at EL1. */
#define SCTLR_EE_LITTLE_ENDIAN	(0 << 25)
/** Work only with little-endian format at EL0. */
#define SCTLR_EOE_LITTLE_ENDIAN	(0 << 24)
/** Disable instruction cache. */
#define SCTLR_I_CACHE_DISABLED	(0 << 12)
/** enable instruction cache. */
#define SCTLR_I_CACHE_ENABLED	(1 << 12)
/** Disable data cache. */
#define SCTLR_D_CACHE_DISABLED	(0 << 2)
/** enable data cache. */
#define SCTLR_D_CACHE_ENABLED	(1 << 2)
/** Disable MMU. */
#define SCTLR_MMU_DISABLED	(0 << 0)
/** SCTLR_MMU_ENABLED */
#define SCTLR_MMU_ENABLED	(1 << 0)
/** All other SCTLR options or'ed. */
#define SCTLR_VALUE_MMU_DISABLED	(SCTLR_RESERVED | SCTLR_EE_LITTLE_ENDIAN | SCTLR_I_CACHE_DISABLED | SCTLR_D_CACHE_DISABLED | SCTLR_MMU_DISABLED)
/** @} */

#define SCTLR_VALUE_MMU_ENABLED (SCTLR_MMU_ENABLED | SCTLR_I_CACHE_ENABLED |SCTLR_D_CACHE_ENABLED)
/** @} */

/**
 * \ingroup entry8
 * \name HCR_EL2
 * @{
 * HCR_EL2, Hypervisor Configuration Register (EL2) \n
 * Section D13.2.48 of AArch64-Reference-Manual.
 */
/** \ingroup entry8 Sets execution state at next lower level to be AArch64. */
#define HCR_RW			(1 << 31)
#define HCR_API         (1 << 41)
#define HCR_APk         (1 << 40)

#define HCR_HOST_NVHE_FLAGS (HCR_RW | HCR_API | HCR_APk)

/** All other HCR options or'ed. */
#define HCR_VALUE		HCR_RW
/** @} */

/**
 * \ingroup entry8
 * \name SCR_EL3
 * @{
 * SCR_EL3, Secure Configuration Register (EL3) \n
 * Section G8.2.125 of AArch64-Reference-Manual.
 */
/** \ingroup entry8 Reserved bits are initialized to 1. */
#define SCR_RESERVED                    (3 << 4)
/** Sets execution state at next lower level to be AArch64. */
#define SCR_RW                          (1 << 10)
/** EL0 and EL1 are in Non-secure state, memory accesses,
 * from those exception levels cannot access Secure memory. */
#define SCR_NS                          (1 << 0)
/** All other SCR options or'ed. */
#define SCR_VALUE                       (SCR_RESERVED | SCR_RW | SCR_NS)
/** @} */

/**
 * \ingroup entry8
 * \name SPSR_EL3
 * @{
 * SPSR_EL3, Saved Program Status Register (EL3) \n
 * Section G8.2.128 of AArch64-Reference-Manual.
 */

/** \ingroup entry8 After we change EL to EL1 all types of interrupts will be masked (disabled). */
#define SPSR_MASK_ALL 		(7 << 6)
/** EL1h mode: We are using EL1 dedicated stack pointer. */
#define SPSR_EL1h		(5 << 0)
#define SPSR_EL2h		(9 << 0)

#define SPSR_EL1 (SPSR_MASK_ALL | SPSR_EL1h)
#define SPSR_EL2 (SPSR_MASK_ALL | SPSR_EL2h)

/** All other SPSR options or'ed. */
#define SPSR_VALUE		(SPSR_MASK_ALL | SPSR_EL1h)
/** @} */

/* Current Exception Level values, as contained in CurrentEL */
#define CurrentEL_EL1		(1 << 2)
#define CurrentEL_EL2		(2 << 2)
#define CurrentEL_EL3		(3 << 2)

// translation control register
#define TCR_T0SZ(n)   ((n) & 0x3f)
#define TCR_IRGN0(n)  (((n) & 0x3) << 8)
#define TCR_ORGN0(n)  (((n) & 0x3) << 10)
#define TCR_SH0(n)    (((n) & 0x3) << 12)
#define TCR_TG0(n)    (((n) & 0x3) << 14)
#define TCR_T1SZ(n)   (((n) & 0x3f) << 16)
#define TCR_IRGN1(n)  (((n) & 0x3) << 24)
#define TCR_ORGN1(n)  (((n) & 0x3) << 26)
#define TCR_SH1(n)    (((n) & 0x3) << 28)
#define TCR_TG1(n)    (((n) & 0x3) << 30)
#define TCR_IPS(n)    (((n) & 0x7) << 32)
#define TCR_AS        (1 << 36)
#define TCR_TBI0      (1 << 37)
#define TCR_ENABLE (TCR_T0SZ(25)|TCR_T1SZ(25)|TCR_TG0(0)|TCR_TG1(2)|TCR_IPS(0)| \
                  TCR_IRGN0(1)|TCR_ORGN0(1)|TCR_SH0(3)| \
                  TCR_IRGN1(1)|TCR_ORGN1(1)|TCR_SH1(3)| \
                  TCR_AS|TCR_TBI0)


// attribute index
// index is set by mair_el1
#define AI_DEVICE_nGnRnE_IDX  0
#define AI_NORMAL_NC_IDX      1
#define AI_NORMAL_CACHE_IDX   2

// memory type
#define MT_DEVICE_nGnRnE  0x0
#define MT_NORMAL_NC      0x44
#define MT_NORMAL_CACHE   0xff
#define MAIR_ENABLE  ((MT_DEVICE_nGnRnE<<(8*AI_DEVICE_nGnRnE_IDX)) | (MT_NORMAL_NC<<(8*AI_NORMAL_NC_IDX)) | \
                  (MT_NORMAL_CACHE<<(8*AI_NORMAL_CACHE_IDX)))

#endif