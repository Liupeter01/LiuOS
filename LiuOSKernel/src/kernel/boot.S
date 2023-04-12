#include <kernel/mmu.h>
#include <kernel/page.h>

.section ".text.boot"		//img文件的第一个分区
.global _start
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
	//adrp x0,init_stktop			//获取栈的绝对地址
	mov sp, 0x80000					// Setup the stack (64 bit).

	ldr x5, =__bss_start		// Clear out BSS.
	ldr w6, =__bss_size

clear_bss:
	cbz w6, create_identical_mapping
	str xzr, [x5], #8
	sub w6, w6, #1
	cbnz w6, clear_bss

create_identical_mapping:
	adrp x0, _U_l2_pgtbl 				//USER L2 PMD
	adrp x1, __start_init				//__start_init pa(va)
	adrp x2, __end_init					//__end_init   pa(va)

	/*用于计算PMD(L2)页表的下标*/
	lsr x3, x1, #21						//计算__start_init PMD偏移 >> 21	
	and x3, x3, #0x1FF					//x3 & 1FF							//PMD 0
	lsr x4, x2, #21						//计算__end_init PMD偏移  >> 21		
	and x4, x4, #0x1FF					//x3 & 1FF							//PMD 1

	/*用于计算PMD(L2)页表的属性*/
	ldr x5 , =NORMAL_MEMORY_BLOCK_MAPPING //block entry
	and x7, x1, #PMD_MASK				//phys = phys & PMD_MASK

identical_segment_loop:
	orr x6, x7, x5						//段入口(结合属性和物理地址)
	str x6 ,[x0, x3 , lsl #3]			//_U_l2_pgtbl[x3 * 8] = x6
	add x3 ,x3 ,#1						//x3+=1
	add x7 ,x7 ,#INIT_KERN_SZ			//X6+=0x200000
	cmp x3 ,x4
	b.ls identical_segment_loop 		//__start_init < __end_init

	adrp x0, idmap_pg_dir				//USER L1 PGD(L0)
	lsr x3, x1 , #PXSHIFT(1)			//访问PGD编号 >> 30
	and x3, x3, #PXMASK					//x3 & 1FF							//PGD 0
	mov x4, #(PTE_TABLE | PTE_VALID)	//level0 转换表不支持Block描述符，尽可以使用0b11
	adrp x5, _U_l2_pgtbl
	orr x6, x4, x5						//PMD的入口地址，用于将PGD指向PMD
	str x6, [x0 , x3 , lsl #3]			//PGD[x3 *8] = PMD

create_kernel_linear:
	adrp x0, _K_l2_pgtbl 				//KERNEL L2 PMD
	adrp x3, __kernel_size				//kernel size
	adrp x1,  __end_init				// start pa
	add x2, x1, x3						// end pa

	ldr x4, =(KERNBASE + PHY_START) 		//start va
	add x5, x4, x3						// end va

	/*用于计算PMD(L2)页表的下标*/
	lsr x6, x4, #PXSHIFT(2)				//计算__start_kernel  
	and x6, x6, #0x1FF					//x6 & 1FF				//PMD偏移 0
	lsr x7, x5, #PXSHIFT(2)				//计算__end_kernel 
	and x7, x7, #0x1FF					//x7 & 1FF				// PMD偏移 1

	/*用于计算PMD(L2)页表的属性*/
	ldr x8 , =NORMAL_MEMORY_BLOCK_MAPPING //block entry
	and x9, x1, #PMD_MASK				//phys = phys & PMD_MASK

linear_segment_loop:
	orr x10, x8, x9						//段入口(结合属性和物理地址)
	str x10 ,[x0, x6 , lsl #3]			//_K_l2_pgtbl[x6 * 8] = x10
	add x6 ,x6 ,#1						//x6 += 1
	add x9 ,x9 ,#INIT_KERN_SZ			//X9 += 0x200000
	cmp x6, x7
	b.ls linear_segment_loop			//__start_kernel < __end_kernel

	adrp x0, swapper_pg_dir				//KERNEL L1 PGD(0~1)
	lsr x5, x4 ,#PXSHIFT(1)			    //访问PGD编号(511)
	and x5, x5, #PXMASK					//x5 & 1FF				//PGD[511]

	mov x6, #(PTE_TABLE | PTE_VALID)    // entry attr
	adrp x7, _K_l2_pgtbl
	orr x8, x6, x7

	str x8 , [x0, x5, lsl #3]			//swapper_pg_dir[x5 * 8] = _K_l2_pgtbl
	
create_gpio_mapping:
	adrp x0, _D_l2_pgtbl 				//KERNEL L2 PMD
	mov x1, #MMIO_BASE 					// start mmio pa
    mov x2, #MMIO_END-1   				// end mmio pa
	mov x3, #KERNBASE
	add x4, x1, x3						// start mmio va
	add x5, x2, x3						// end mmio va

	lsr x6, x4, #PXSHIFT(2)				//计算start mmio va PMD偏移(1F0)
	and x6, x6, #0x1FF					//x6 & 1FF

	lsr x7, x5, #PXSHIFT(2)				//计算end mmio va  PMD偏移
	and x7, x7, #0x1FF					//x7 & 1FF

	mov x8, #(PTE_AF | PTE_DEVICE | PTE_VALID) //设备内存
	orr x9, x1, x8      // block entry

gpio_segment_loop:
	str x9 ,[x0, x6 , lsl #3]			//_K_l2_pgtbl[x6 * 8] = x9
	add x6 ,x6 ,#1						//x6 += 1
	add x9 ,x9 ,#INIT_KERN_SZ			//X9 += 0x200000
	cmp x6, x7
	b.ls gpio_segment_loop				//start mmio va < end mmio va

	adrp x0, swapper_pg_dir				//KERNEL L1 PGD
	lsr x5, x4 , #PXSHIFT(1)			//访问PGD编号
	and x5, x5, #PXMASK					//x5 & 1FF

	mov x6, #(PTE_TABLE | PTE_VALID)    // entry attr
	adrp x7, _D_l2_pgtbl
	orr x8, x6, x7

	str x8 , [x0, x5, lsl #3]			//swapper_pg_dir[x5 *8] = _D_l2_pgtbl

load_mmu:
	adrp x0, idmap_pg_dir
    adrp x1, swapper_pg_dir
	msr ttbr0_el1, x0
	msr ttbr1_el1, x1
	dsb	ishst
    tlbi vmalle1is
    dsb	ish
    isb
	ldr x0, =MAIR_ENABLE
	msr mair_el1, x0

	ldr x1,=TCR_ENABLE
	msr tcr_el1, x1

	/*开启MMU*/
	mrs x0, sctlr_el1
	ldr x1,=SCTLR_VALUE_MMU_ENABLED
	orr x0, x0, x1
	msr sctlr_el1, x0

	ic iallu
	dsb nsh
	isb

	LDR     x1, =kernel_stack
	mov     sp, x1

	b main  			// Call main (C Code).
						// Should not return from here.
	b proc_hang			// For failsafe, halt this core too.