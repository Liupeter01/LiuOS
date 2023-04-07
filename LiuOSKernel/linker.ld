OUTPUT_FORMAT("elf64-littleaarch64")
OUTPUT_ARCH(aarch64)
ENTRY(_start)

PHY_START = 0x80000;
KERNERL_BASE = 0xFFFFFFFF00000000;
INIT_STACK_SIZE = 0x1000;

SECTIONS
{
    . = PHY_START; /*程序链接地址=加载地址*/
    .text : 
    { 
      KEEP(*(.text.boot))
       *(.text .text.* .gnu.linkonce.t*) 
    }
    .rodata : 
    { 
      *(.rodata .rodata.* .gnu.linkonce.r*) 
    }
    . = ALIGN(4096);
    PROVIDE(_data = .);
    .data : 
    { 
      *(.data .data.* .gnu.linkonce.d*) 
    }
    .bss : 
    {
        . = ALIGN(16);
        __bss_start = .;
        *(.bss .bss.*)
        *(COMMON)
        __bss_end = .;
        
      /*linker.ld*/
      . += INIT_STACK_SIZE;       /*预留栈空间的大小为4096B*/
      PROVIDE(init_stktop = .);   /*预留的SP指针物理地址位置，栈向低地址生长*/

      . = ALIGN(0x1000);
      /* 内核PUD页表，PUD有512个条目，每个条目8B*/
      PROVIDE (_kernel_pgtbl = .);
      . += 0x1000;
     /* 用户PUD页表，存储原理同上*/
      PROVIDE (_user_pgtbl = .);
       . += 0x1000;
      /*内核PMD页表，PUD有512个条目，指向512个大小为4096B的PMD*/
      PROVIDE (_K_l2_pgtbl = .);
      . += 0x200000;
      /*用户PMD页表，PUD有512个条目，指向512个大小为4096B的PMD*/
      PROVIDE (_U_l2_pgtbl = .);
      . += 0x200000;
    }
    . = ALIGN(4096);
    PROVIDE (__end = .);
}
__bss_size = ( __bss_end - __bss_start) >> 3;