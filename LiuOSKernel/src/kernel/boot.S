﻿#include <armv8-a/sysregs.h>
#include <armv8-a/memlayout.h>

.section ".text.boot"		//img文件的第一个分区
.global _start

/*
 * Entry point for the kernel
 * x0 -> 32 bit pointer to DTB in memory (primary core only) / 0 (secondary cores)
 * x1 -> 0
 * x2 -> 0
 * x3 -> 0
 * x4 -> 32 bit kernel entry point, _start location
 * Preserve these registers as argument for kernel_main
 */

_start:
	/* Check if processor ID is zero (executing on main core), else hang */
	mrs x0, mpidr_el1
	and x0, x0, #0xFF	// Check processor id
	cbz x0, master		// Hang for all non-primary CPU
	b proc_hang
	/* We are not on the main core, so hang in an infinite wait loop */
proc_hang:
	b proc_hang

master:
	/* System Control Register (SCTLR)
	 * Configuring different parameters for the processor.
	 * See <armv8-a/sysregs.h> for configuration.
	 */
	ldr x0, =SCTLR_VALUE_MMU_DISABLED
	msr sctlr_el1, x0


	/* Hypervisor Configuration Register (HCR)
	 * Setting execution state at EL1 to be AArch64.
	 */
	ldr x0, =HCR_VALUE
	msr hcr_el2, x0

	/* Secure Configuration Register (SCR)
	 * Setting execution state at EL2 to be AArch64,
	 * and that all lower exception levels will be "non secure".
	 */
	ldr x0, =SCR_VALUE
	msr scr_el3, x0

	/* Saved Program Status Register (SPSR)
	 * Preparing processor state.
	 * See <armv8-a/sysregs.h> for configuration.
	 */
	ldr x0, =SPSR_VALUE
	msr spsr_el3, x0

	/* Exception Link Register (ELR)
	 * Holds the address, to which we are going to return
	 * after eret instruction will be executed.
	 * Here we set this address to the location of el1_entry label.
	 */
	adr x0, el1_entry
	msr elr_el3, x0

	// enable CNTP for EL1
    mrs     x0, cnthctl_el2
    orr     x0, x0, #3
    msr     cnthctl_el2, x0
    msr     cntvoff_el2, xzr
	eret

el1_entry:
	/* Code executing only by main core (cpu id == 0) 
	 * With the latest firmware, only the primary core runs (core 0), and
	 * the secondary cores are awaiting in a spin loop.
	 */
	adrp x0,init_stktop	//获取栈的绝对地址
	mov sp, x0		// Setup the stack (64 bit).

	ldr x5, =__bss_start		// Clear out BSS.
	ldr w6, =__bss_size

clear_bss:
	cbz w6, kernel_entry
	str xzr, [x5], #8
	sub w6, w6, #1
	cbnz w6, clear_bss

kernel_entry:
	bl mmu_init  	// Call mmu_init (C Code).
					// Should not return from here.

	b proc_hang			// For failsafe, halt this core too.

.global jump_stack
jump_stack:
	MOV     x0, sp
	LDR     x1, =KERNBASE
	ADD     sp, x1, x0
	RET