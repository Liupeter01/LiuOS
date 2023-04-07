#ifndef _AARCH64_H_
#define _AARCH64_H_
#pragma once
#include <stdint.h>
#include <armv8-a/basetype.h>

#ifndef __ASSEMBLER__
// which core is this?
static inline uint64
cpuid()
{
  uint64 x;
  asm volatile("mrs %0, mpidr_el1" : "=r" (x) );
  return x & 0xff;
}

// Vector Base Address Register in EL1
static inline void
w_vbar_el1(uint64 x)
{
  asm volatile("msr vbar_el1, %0" : : "r" (x) );
}

static inline uint64
r_vbar_el1()
{
  uint64 x;
  asm volatile("mrs %0, vbar_el1" : "=r" (x) );
  return x;
}

static inline uint64
r_par_el1()
{
  uint64 x;
  asm volatile("mrs %0, par_el1" : "=r" (x) );
  return x;
}

// TODO: fix comment
// supervisor address translation and protection;
// holds the address of the page table.
static inline void
w_ttbr0_el1(uint64 x)
{
  asm volatile("msr ttbr0_el1, %0" : : "r" (x) );
}

static inline uint64
r_ttbr0_el1()
{
  uint64 x;
  asm volatile("mrs %0, ttbr0_el1" : "=r" (x) );
  return x;
}

static inline void
w_ttbr1_el1(uint64 x)
{
  asm volatile("msr ttbr1_el1, %0" : : "r" (x));
}

static inline uint64
r_ttbr1_el1()
{
  uint64 x;
  asm volatile("mrs %0, ttbr1_el1" : "=r" (x) );
  return x;
}

static inline uint64
r_esr_el1()
{
  uint64 x;
  asm volatile("mrs %0, esr_el1" : "=r" (x) );
  return x;
}

static inline void
w_esr_el1(uint64 x)
{
  asm volatile("msr esr_el1, %0" : : "r" (x) );
}

static inline uint64
r_elr_el1()
{
  uint64 x;
  asm volatile("mrs %0, elr_el1" : "=r" (x) );
  return x;
}

static inline uint64
r_far_el1()
{
  uint64 x;
  asm volatile("mrs %0, far_el1" : "=r" (x) );
  return x;
}

// armv8 generic timer
static inline uint64
r_cntv_ctl_el0()
{
  uint64 x;
  asm volatile("mrs %0, cntv_ctl_el0" : "=r" (x) );
  return x;
}

static inline void
w_cntv_ctl_el0(uint64 x)
{
  asm volatile("msr cntv_ctl_el0, %0" : : "r" (x) );
}

static inline uint64
r_cntv_tval_el0()
{
  uint64 x;
  asm volatile("mrs %0, cntv_tval_el0" : "=r" (x) );
  return x;
}

static inline void
w_cntv_tval_el0(uint64 x)
{
  asm volatile("msr cntv_tval_el0, %0" : : "r" (x) );
}

static inline uint64
r_cntvct_el0()
{
  uint64 x;
  asm volatile("mrs %0, cntvct_el0" : "=r" (x) );
  return x;
}

static inline uint64
r_cntfrq_el0()
{
  uint64 x;
  asm volatile("mrs %0, cntfrq_el0" : "=r" (x) );
  return x;
}

// enable device interrupts(irq)
static inline void
intr_on()
{
  asm volatile("msr daifclr, #0xf" ::: "memory");
}

// disable device interrupts(irq)
static inline void
intr_off()
{
  asm volatile("msr daifset, #0xf" ::: "memory");
}

static inline uint64
daif()
{
  uint64 x;
  asm volatile("mrs %0, daif" : "=r" (x) );
  return x;
}

// are device interrupts(irq) enabled?
static inline int
intr_get()
{
  uint64 x = daif();
  return ((x >> 6) & 0x2) == 0;
}

static inline uint64
r_sp()
{
  uint64 x;
  asm volatile("mov %0, sp" : "=r" (x) );
  return x;
}

static inline uint64
r_ra()
{
  uint64 x;
  asm volatile("mov %0, x30" : "=r" (x) );
  return x;
}

static inline void
isb()
{
  asm volatile("isb");
}

#define dsb(ty)   asm volatile("dsb " #ty)

// flush the TLB.
static inline void
flush_tlb()
{
  asm volatile("dsb ishst");
  asm volatile("tlbi vmalle1is");
  asm volatile("dsb ish");
  isb();
}

/**
 * \ingroup drivers
 * Memory-Mapped I/O output.
 * @param reg 64-bit register address.
 * @param data 32-bit data.
 * @details Writes the @a data value to the register at @a reg address.
*/
void mmio_write(uint64_t reg, uint32_t data);

/**
 * \ingroup drivers
 * Memory-Mapped I/O input.
 * @param reg 64-bit register address.
 * @return 32-bit data.
 * @details Reads register at @a reg address and returns its @a data.
*/
uint32_t mmio_read(uint64_t reg);

/**
 * \ingroup drivers
 * Delays @a count of clock cycles.
 * @param count Number of cycles.
 * Loop <count> times, so the compiler won't optimize away
*/
void delay(int32_t count);

typedef uint64 pte_t;
typedef uint64 *pagetable_t; // 512 PTEs

#endif  /* __ASSEMBLER__ */

#define NPROC        64  // maximum number of processes
#define NCPU          4  // maximum number of CPUs
#define NOFILE       16  // open files per process
#define NFILE       100  // open files per system
#define NINODE       50  // maximum number of active i-nodes
#define NDEV         10  // maximum major device number
#define ROOTDEV       1  // device number of file system root disk
#define MAXARG       32  // max exec arguments
#define MAXOPBLOCKS  10  // max # of blocks any FS op writes
#define LOGSIZE      (MAXOPBLOCKS*3)  // max data blocks in on-disk log
#define NBUF         (MAXOPBLOCKS*3)  // size of disk block cache
#define FSSIZE       1000  // size of file system in blocks
#define MAXPATH      128   // maximum file path name

#define PGSIZE 4096 // bytes per page
#define PGSHIFT 12  // bits of offset within a page

#define PGROUNDUP(sz)  (((sz)+PGSIZE-1) & ~(PGSIZE-1))
#define PGROUNDDOWN(a) (((a)) & ~(PGSIZE-1))

#define SECTSIZE  (2*1024*1024)   // 2MB
#define SECTROUNDUP(sz)  (((sz)+SECTSIZE-1) & ~(SECTSIZE-1))

#endif //_AARCH64_H_