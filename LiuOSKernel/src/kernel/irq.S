/*
 * irq.S - Implementation of armv8-a interrupt initialization functions.
 */

#include <armv8-a/entry.h>

/*
 * Macro for creating entries in the vector table.
 * Jump to the label that is provided for the macro as label argument.
 * All except. vectors should be located at offset 0x80 bytes one from another.
 */
.macro ventry label
.align 7
	b \label
.endm

/*
 * Macro for storing registers x0 - x30 to the stack, at the start of an
 * interrupt.
 */
	/* stp: Stores a pair of registers */
.macro kernel_entry
	sub sp,  sp,  #S_FRAME_SIZE
	stp x0,  x1,  [sp, #16 * 0]
	stp x2,  x3,  [sp, #16 * 1]
	stp x4,  x5,  [sp, #16 * 2]
	stp x6,  x7,  [sp, #16 * 3]
	stp x8,  x9,  [sp, #16 * 4]
	stp x10, x11, [sp, #16 * 5]
	stp x12, x13, [sp, #16 * 6]
	stp x14, x15, [sp, #16 * 7]
	stp x16, x17, [sp, #16 * 8]
	stp x18, x19, [sp, #16 * 9]
	stp x20, x21, [sp, #16 * 10]
	stp x22, x23, [sp, #16 * 11]
	stp x24, x25, [sp, #16 * 12]
	stp x26, x27, [sp, #16 * 13]
	stp x28, x29, [sp, #16 * 14]

	/*
	 * Because of context switching, it is possible to enter the interrupt
	 * as one task, and exit as a different one.
	 * eret, relies on the fact that return address is stored in elr_el1
	 * and processor state in spsr_el1.
	 * In order to switch tasks will processing an interrupt, we must save
	 * these 2 registers, alongside the other general purpose registers.
	 */
	mrs x22, elr_el1
	mrs x23, spsr_el1

	stp x30, x22, [sp, #16 * 15]
	str x23,      [sp, #16 * 16]
.endm

/*
 * Macro for restoring the processor state by copying back the values of
 * x0 - x30 registers at the end of an interrupt.
 */
	/* ldp: Loads a pair of registers */
.macro kernel_exit

	/*
	 * Because of context switching, we do the opposite from kernel_entry.
	 * We restore elr_el1 and spsr_el1, before restoring the other
	 * general purpose registers.
	 */
	ldr x23,      [sp, #16 * 16]
	ldp x30, x22, [sp, #16 * 15]

	msr elr_el1, x22
	msr spsr_el1, x23

	ldp x0,  x1,  [sp, #16 * 0]
	ldp x2,  x3,  [sp, #16 * 1]
	ldp x4,  x5,  [sp, #16 * 2]
	ldp x6,  x7,  [sp, #16 * 3]
	ldp x8,  x9,  [sp, #16 * 4]
	ldp x10, x11, [sp, #16 * 5]
	ldp x12, x13, [sp, #16 * 6]
	ldp x14, x15, [sp, #16 * 7]
	ldp x16, x17, [sp, #16 * 8]
	ldp x18, x19, [sp, #16 * 9]
	ldp x20, x21, [sp, #16 * 10]
	ldp x22, x23, [sp, #16 * 11]
	ldp x24, x25, [sp, #16 * 12]
	ldp x26, x27, [sp, #16 * 13]
	ldp x28, x29, [sp, #16 * 14]
	add sp, sp, #S_FRAME_SIZE
	eret
.endm

/* For IRQ EL0h: system call */
el0_irq:
	kernel_entry
	/*bl handle_irq*/
	kernel_exit

/* For IRQ EL1h: We call the exception handler handle_irq from C code. */
el1_irq:
	kernel_entry
	bl handle_irq
	kernel_exit

/*
 * Function for putting the processor in an infinite loop, in case of error.
 */
.globl err_hang
err_hang:
	b err_hang

 /*
 * Exception vector table.
 * 16 entries created with the ventry macro.
 * For each exception we jump to the corresponding label (handler).
 */
.align 11
.globl vectors
vectors:
	ventry sync_invalid_el1t	/* Synchronous EL1t */
	ventry irq_invalid_el1t		/* IRQ EL1t         */
	ventry fiq_invalid_el1t		/* FIQ EL1t         */
	ventry error_invalid_el1t	/* Error EL1t       */

	ventry sync_invalid_el1h	/* Synchronous EL1h */
	ventry el1_irq			    /* !!!! IRQ EL1h !!!!*/
	ventry fiq_invalid_el1h		/* FIQ EL1h         */
	ventry error_invalid_el1h	/* Error EL1h       */

	ventry sync_invalid_el0_64	/* Synchronous 64-bit EL0 */
	ventry el0_irq				 /* !!!! IRQ EL0h !!!!*/
	ventry fiq_invalid_el0_64	/* FIQ 64-bit EL0         */
	ventry error_invalid_el0_64	/* Error 64-bit EL0       */

	ventry sync_invalid_el0_32	/* Synchronous 32-bit EL0 */
	ventry irq_invalid_el0_32	/* IRQ 32-bit EL0         */
	ventry fiq_invalid_el0_32	/* FIQ 32-bit EL0         */
	ventry error_invalid_el0_32	/* Error 32-bit EL0       */

/*
 * Macro for handlers that should never be executed in normal flow.
 * We call the C function show_invalid_entry_message, in order to print
 * information about the error, and the processor is put in an infinite loop.
 *
 * #\type:  Invalid type defined in <armv8-a/entry.h>
 * esr_el1: Exception Syndrome Register (ESR) contains detailed information
 		about what causes an exception.
 * elr_el1: Exception Link Register (ELR) contains the address of the
 		instruction that had been executed when the exception
		was generated (for synchronous interrupts).
 */
.macro handle_invalid_entry type
	kernel_entry
	mov x0, #\type
	mrs x1, esr_el1
	mrs x2, elr_el1
	bl show_invalid_entry_message
	b err_hang
.endm

/*
 * Implementation of exception handlers.
 */
 /* For other exceptions: We call the handle_invalid_entry macro,
	with the corresponding invalid exception type value. */
sync_invalid_el1t:
	handle_invalid_entry SYNC_INVALID_EL1t

irq_invalid_el1t:
	handle_invalid_entry IRQ_INVALID_EL1t

fiq_invalid_el1t:
	handle_invalid_entry FIQ_INVALID_EL1t

error_invalid_el1t:
	handle_invalid_entry ERROR_INVALID_EL1t

sync_invalid_el1h:
	handle_invalid_entry SYNC_INVALID_EL1h

fiq_invalid_el1h:
	handle_invalid_entry FIQ_INVALID_EL1h

error_invalid_el1h:
	handle_invalid_entry ERROR_INVALID_EL1h

sync_invalid_el0_64:
	handle_invalid_entry SYNC_INVALID_EL0_64

fiq_invalid_el0_64:
	handle_invalid_entry FIQ_INVALID_EL0_64

error_invalid_el0_64:
	handle_invalid_entry ERROR_INVALID_EL0_64

sync_invalid_el0_32:
	handle_invalid_entry SYNC_INVALID_EL0_32

irq_invalid_el0_32:
	handle_invalid_entry IRQ_INVALID_EL0_32

fiq_invalid_el0_32:
	handle_invalid_entry FIQ_INVALID_EL0_32

error_invalid_el0_32:
	handle_invalid_entry ERROR_INVALID_EL0_32

/*
 * Initializes interrupts, by setting the address of the Vector Base Address
 * 	Register (vbar_el1) to our vector table address.
 */
.globl irq_vector_init
irq_vector_init:
	adr x0, vectors		/* load VBAR_EL1 with virtual */
	msr vbar_el1, x0	/* vector table address */
	ret

/*
 * Enables (Unmasks) interrupts.
 */
.globl irq_enable
irq_enable:
	msr daifclr, #2		/* Unmask IRQs */
	ret

/*
 * Disables (Masks) interrupts.
 */
.globl irq_disable
irq_disable:
	msr daifset, #2		/* Mask IRQs */
	ret

/*
 * DAIF{clr,set}:
 *	D: Un/Masks debug exceptions	(Bit 0)
 *	A: Un/Masks SErrors		(Bit 1)
 *	I: Un/Masks IRQs		(Bit 2)
 *	F: Un/Masks FIQs		(Bit 3)
 */
