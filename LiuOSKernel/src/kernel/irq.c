/**
 * \ingroup exceptions
 * @file irq.c
 * @brief Implementation of interrupt handling functions.
 */

#include <kernel/irq.h>

/**
 * irq Base Address, we consider about vitual mmio mapping problem
 */
volatile uchar *kernel_irq_base;

/**
 * String array, for the invalid exception type messages.
 */
const char entry_error_messages[16][32] = {
	"SYNC_INVALID_EL1t",
	"IRQ_INVALID_EL1t",
	"FIQ_INVALID_EL1t",
	"ERROR_INVALID_EL1T",

	"SYNC_INVALID_EL1h",
	"IRQ_INVALID_EL1h",
	"FIQ_INVALID_EL1h",
	"ERROR_INVALID_EL1h",

	"SYNC_INVALID_EL0_64",
	"IRQ_INVALID_EL0_64",
	"FIQ_INVALID_EL0_64",
	"ERROR_INVALID_EL0_64",

	"SYNC_INVALID_EL0_32",
	"IRQ_INVALID_EL0_32",
	"FIQ_INVALID_EL0_32",
	"ERROR_INVALID_EL0_32"
};

/**
 * Enables System Timer and UART interrupts.
 * @see peripherals/irq.h
 */
void enable_interrupt_controller()
{
	kernel_irq_base = &kernel_gpio_base[IRQ_BASE];
	mmio_write(
		(uint64_t)(&kernel_irq_base[IRQ0_SET_EN_0]), 			//使能中断控制器
		/* AUX_IRQ | */ 
		SYSTEM_TIMER_IRQ_1 | SYSTEM_TIMER_IRQ_3		//启动两个时钟中断(一个控制时间片另一个用于其他用途)
	);
}

void show_invalid_entry_message(int type, uint64_t esr, uint64_t address)
{
	printk("%s, ESR: %x, address: %x\n", \
			entry_error_messages[type], esr, address);
}


void handle_irq()
{
	uint32_t irq = mmio_read((uint64_t)(&kernel_irq_base[IRQ0_PENDING_0]));

	/* While we have a valid value from the interrupt */
	while (irq) {
		/* Check which interrupt is it */

		if (irq & SYSTEM_TIMER_IRQ_1) {
			/* Remove the bit we handled */
			irq &= ~SYSTEM_TIMER_IRQ_1;
			handle_timer_1_irq();
		}
		else if (irq & SYSTEM_TIMER_IRQ_3) {
			/* Remove the bit we handled */
			irq &= ~SYSTEM_TIMER_IRQ_3;
			handle_timer_3_irq();
		}
		else if (irq & AUX_IRQ) {
			/* Remove the bit we handled */
			irq &= ~AUX_IRQ;
			handle_uart_irq();
		}
		else {
			printk("Unknown pending irq: %x\n", irq);
		}
	}
}
