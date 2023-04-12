OUTPUT_FORMAT("elf64-littleaarch64")
OUTPUT_ARCH(aarch64)
ENTRY(_start)

PHY_START = 0x80000;
KERNERL_BASE = 0xffffff8000000000	;
/*INIT_STACK_SIZE = 0x1000;*/

SECTIONS
{
    . = PHY_START;
    .init : {
       PROVIDE (__start_init = .);
       KEEP(*(.text.boot))
        . = ALIGN(16);
        PROVIDE (__bss_start = .);

        /*. += INIT_STACK_SIZE;       /*预留栈空间的大小为4096B*/
        /*PROVIDE(init_stktop = .);   /*预留的SP指针物理地址位置，栈向低地址生长*/

        PROVIDE (__bss_end = .);

        . = ALIGN(0x1000);
        /*初始阶段的页表（PGD/PUD/PMD/PTE）都是排列在一起的，每一个占用一个page*/
        idmap_pg_dir = .;
        . += 0x1000;
        /* 内核PUD页表，PUD有512个条目，每个条目8B*/
        swapper_pg_dir = .;
        . += 0x1000;

        _U_l2_pgtbl = .;
        . += 0x1000;
        /*内核PMD页表，PUD有512个条目，指向512个大小为4096B的PMD*/
        _K_l2_pgtbl = .;
        . += 0x1000;

        _D_l2_pgtbl = .;
        . += 0x1000;
        
        __end_init = .;
    }

    . = KERNERL_BASE + __end_init; 
     PROVIDE (__start_kernel = .);
    .text : AT(__end_init)
    { 
        EXCLUDE_FILE (*build/boot_s.o) *(.text .text.*) 
    }
    .rodata : 
    { 
       EXCLUDE_FILE (*build/boot_s.o) *(.rodata .rodata.*) 
    }
    . = ALIGN(4096);
    .data : 
    { 
       EXCLUDE_FILE (*build/boot_s.o) *(.data .data.*) 
    }
    .bss : 
    {
        . = ALIGN(16);
        EXCLUDE_FILE (*build/boot_s.o) *(.bss .bss.*)
    }
    . = ALIGN(4096);
    __end_kernel = .;
}
__bss_size = ( __bss_end - __bss_start) >> 3;
__kernel_size = __end_kernel - __start_kernel;