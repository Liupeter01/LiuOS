//#include <driver/led.h>
#include <armv8-a/irq.h>
#include <kernel/console.h>
#include <kernel/printk.h>
#include <kernel/mmu.h>
#include <kernel/utils.h>
#include <kernel/timer.h>
//#include <kernel/irq.h>
#include <kernel/mmu.h>

void main(void)
{
	if(cpuid() == 0){
		/* String for device name */
		//char device[DEVICE_LENGTH];
		/* mini UART */
		uart_init((uint64_t)P2V(GPIO_BASE));				//当前映射0xFE000000
		gic_init((uint64_t)P2V(GPIO_BASE));				    //当前映射0xFE015000
		//mmu_init();
		//uart_init(P2V_WO(GPIO_BASE));				//当前MMU已经启动，映射0xFE000000+KERNBASE

		printk("\n\n");
		printk(" _     _        ___  ____  \n");
		printk("| |   (_)_   _ / _ \\/ ___| \n");
		printk("| |   | | | | | | | \\___ \\ \n");
		printk("| |___| | |_| | |_| |___) |\n");
		printk("|_____|_|\\__,_|\\___/|____/ \n");                
		printk("\nLiuOS Starting...\n");
		
	}
} 