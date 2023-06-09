/**
 * \defgroup exceptions exceptions
 * Code for handling exceptions/interupts.
 * @{
 * @file irq.h
 * @brief Definition of interrupt handling functions.
 */

#ifndef IRQ_H
#define IRQ_H

#include <stdint.h>
#include <kernel/printk.h>
#include <driver/uart.h>
#include <kernel/timer.h>
#include <peripherals/irq.h>
#include <peripherals/auxiliary.h>

/**
 * Initializes interrupts, by setting the address of the Vector Base Address
 * Register (vbar_el1) to our vector table address.
 * @see irq.S
 */
void irq_vector_init();

/**
 * Enables System Timer and UART interrupts.
 * @see peripherals/irq.h
 */
void enable_interrupt_controller();

/**
 * Prints information about the error, on an invalid exception.
 * @param type Type of exception, defined in include/armv8-a/entry.h
 * @param esr Exception Syndrome Register (ESR),
 * contains detailed information about what causes an exception.
 * @param address The address of the instruction that had been executed
 * when the exception was generated.
 * @see include/armv8-a/entry.h
 */
void show_invalid_entry_message(int type, unsigned long esr, \
						unsigned long address);

/**
 * Exception handler for all IRQs.
 * @details Reads from the interrupt pending registers ( @ref IRQ0_PENDING_0 or
 *  @ref IRQ0_PENDING_1), and if a valid value is found we have an interrupt.
 * Checks each bit for the interrupts we have enabled, and if it is set,
 * it removes the bit we handled and calls the corresponding handler.
 * @see handle_timer_1_irq(), handle_timer_3_irq(), handle_uart_irq()
 */
void handle_irq();

/** 单核CPU开中断 */
void irq_enable();

/** 单核CPU关中断 */
void irq_disable();

/** @} */

#endif