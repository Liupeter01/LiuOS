#include <driver/led.h>
#include <kernel/console.h>
#include <kernel/printk.h>
#include <kernel/mmu.h>
#include <kernel/utils.h>
#include <kernel/timer.h>
#include <kernel/irq.h>
#include <kernel/mmu.h>

void main(void)
{
	/* String for device name */
	//char device[DEVICE_LENGTH];
	irq_vector_init();									//初始化中断向量
	uart_init((uint64_t)P2V(GPIO_BASE));				//当前映射0xFE000000+KERNBASE
	gic_init((uint64_t)P2V(GPIO_BASE));				    //当前映射0xFE015000+KERNBASE
	enable_interrupt_controller();						//开启中断控制器

	printk("\n\n");
	printk(" _     _        ___  ____  \n");
	printk("| |   (_)_   _ / _ \\/ ___| \n");
	printk("| |   | | | | | | | \\___ \\ \n");
	printk("| |___| | |_| | |_| |___) |\n");
	printk("|_____|_|\\__,_|\\___/|____/ \n");                
	printk("\nLiuOS Starting...\n");
} 