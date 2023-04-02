/**
 * \ingroup memory
 * @file mmu.c
 * @brief Implementation of basic memory functions.
 */

#include <kernel/mmu.h>

// get addresses from linker
extern volatile unsigned char _data;
extern volatile unsigned char __end;

uint64 *kernel_pgtbl = &_kernel_pgtbl;
uint64 *user_pgtbl = &_user_pgtbl;

/**
 * Array that holds status for each page in the system.
 * Status:
 * - Allocated	= 1
 * - Free	= 0
 */
static unsigned short mem_map [PAGING_PAGES] = {0,};

uint64_t 
get_free_page()
{
	/* Iterate over all available memory pages */
	for (uint64_t  i = 0; i < PAGING_PAGES; i++) {
		/* If the page is free */
		if (mem_map[i] == 0) {
			/* Mark as allocated */
			mem_map[i] = 1;
			/* Return the starting address of the page */
			return LOW_MEMORY + i * PAGE_SIZE;
		}
	}
	return 0;
}

void 
free_page(uint64_t p)
{
	/* Mark page as free */
	mem_map[(p - LOW_MEMORY) / PAGE_SIZE] = 0;
}

void load_pgtlb (uint64* kern_pgtbl, uint64* user_pgtbl)
{
    uint32	val32;
    uint64	val64;

    //清空指令流水线
    asm("IC IALLUIS": : :);

    //清空TLB
    asm("TLBI VMALLE1" : : :);
    asm("DSB SY" : : :);

    // no trapping on FP/SIMD instructions
    val32 = 0x03 << 20;
    asm("MSR CPACR_EL1, %[v]": :[v]"r" (val32):);

    // monitor debug: all disabled
    asm("MSR MDSCR_EL1, xzr":::);

    // set memory attribute indirection register
    //_puts("Setting Memory Attribute Indirection Register (MAIR_EL1)\n");
    val64 = (uint64)0xFF440C0400;
    asm("MSR MAIR_EL1, %[v]": :[v]"r" (val64):);
    asm("ISB": : :);

    // set vector base address register
   // val64 = (uint64)&vectors;
    //val64 = val64 + (uint64)KERNBASE;
    //asm("MSR VBAR_EL1, %[v]": :[v]"r" (val64):);

    // set translation control register
    val64 = (uint64)0x34B5203520;
    asm("MSR TCR_EL1, %[v]": :[v]"r" (val64):);
    asm("ISB": : :);

    // set translation table base register 1 (kernel)
    val64 = (uint64)kern_pgtbl;
    asm("MSR TTBR1_EL1, %[v]": :[v]"r" (val64):);

    // set translation table base register 0 (user)
    val64 = (uint64)user_pgtbl;
    asm("MSR TTBR0_EL1, %[v]": :[v]"r" (val64):);
    asm("ISB":::);

    asm("MRS %[r], SCTLR_EL1":[r]"=r" (val32): :); //0x0000000000c50838
    val32 = val32 | 0x01;
    asm("MSR SCTLR_EL1, %[v]": :[v]"r" (val32):);
    asm("ISB": : :);
}

// setup the boot page table: dev_mem whether it is device memory
static 
void 
set_bootpgtable(uint64 virt, uint64 phy, uint64 len, int dev_mem )
{
    for (uint64 idx = 0; idx < len; idx = idx + INIT_KERN_SZ) {

        int pgdidx = PGD_IDX(virt);
        int pmdidx = PMD_IDX(virt);

        uint64 pde = phy & PMD_MASK;

        if (!dev_mem) {	// normal memory
            pde |= ACCESS_FLAG | SH_IN_SH | AP_RW_1 | NON_SECURE_PA | MEM_ATTR_IDX_4 | ENTRY_BLOCK | ENTRY_VALID | UXN;
        } else {		  // device memory
            pde |= ACCESS_FLAG | AP_RW_1 | MEM_ATTR_IDX_0 | ENTRY_BLOCK | ENTRY_VALID;
        }

        uint64* level2 = (uint64 *)(kernel_pgtbl[pgdidx] & PG_ADDR_MASK);
        level2[pmdidx] = pde;

        level2 = (uint64 *)(user_pgtbl[pgdidx] & PG_ADDR_MASK);
        level2[pmdidx] = pde;

        virt = virt + 0x200000;
        phy = phy + 0x200000;

    }
}

void 
mmu_init()
{
	/*kernel page table*/
	for(int index = 0; index < PTRS_PER_PGD; index++) {
        uint64 l2pgtbl = (uint64)&_K_l2_pgtbl;
        l2pgtbl += index * PAGE_SIZE;
        l2pgtbl = l2pgtbl | ENTRY_TABLE | ENTRY_VALID;
        kernel_pgtbl[index] = l2pgtbl;
    }

	/*user page table*/
    for(int index = 0; index < PTRS_PER_PGD; index++) {
        uint64 l2pgtbl = (uint64)&_U_l2_pgtbl;
        l2pgtbl += index * PAGE_SIZE;
        l2pgtbl = l2pgtbl | ENTRY_TABLE | ENTRY_VALID;
        user_pgtbl[index] = l2pgtbl;
    }

	//建立页表的恒等映射[PHY_START,end)->[PHY_START,end)
	set_bootpgtable((uint64)PHY_START, (uint64)PHY_START, GET_KERNE_LIMG_SIZE(&__end), FALSE);

	/*建立页表的内核基地址映射[PHY_START,end)->[PHY_START+KERNBASE,P2V(end))*/
	set_bootpgtable((uint64)(PHY_START+KERNBASE), (uint64)PHY_START,  GET_KERNE_LIMG_SIZE(&__end), FALSE);

	/*建立MMIO设备内存的恒等映射[MMIO_BASE,MMIO_END)->[MMIO_BASE,MMIO_END)*/
	set_bootpgtable((uint64)MMIO_BASE, (uint64)MMIO_BASE, (uint64)(MMIO_END - MMIO_BASE), TRUE);

	/*建立MMIO设备内存的内核基地址映射[MMIO_BASE,MMIO_END)->[MMIO_BASE+KERNBASE,MMIO_END+KERNBASE)*/
	set_bootpgtable((uint64)(MMIO_BASE+KERNBASE), (uint64)MMIO_BASE, (uint64)(MMIO_END - MMIO_BASE), TRUE);

	load_pgtlb(kernel_pgtbl,user_pgtbl);

	jump_stack();
}