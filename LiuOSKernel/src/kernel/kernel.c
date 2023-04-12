#include <driver/led.h>
#include <kernel/console.h>
#include <kernel/printk.h>
#include <kernel/mmu.h>
#include <kernel/timer.h>
#include <kernel/irq.h>

char kernel_stack[PAGE_SIZE] = {0};

void main(void)
{
	uart_init((uint64_t)P2V(GPIO_BASE));				//当前映射0xFE000000+KERNBASE
	gic_init((uint64_t)P2V(GPIO_BASE));				    //当前映射0xFE015000+KERNBASE
	setupLogo();										//输出启动图像

	printk("Initalizing IRQs......\n");
	irq_vector_init();									//初始化中断向量
	printk("Initalizing IRQ Controllers......\n");
	enable_interrupt_controller();						//开启中断控制器
} 