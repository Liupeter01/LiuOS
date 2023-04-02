OUTPUT_FORMAT("elf64-littleaarch64")
OUTPUT_ARCH(aarch64)
ENTRY(_start)

SECTIONS
{
    . = 0x80000;
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

      /* define the kernel page table, must be 4K and 4K-aligned*/
      . = ALIGN(0x1000);
      PROVIDE (_kernel_pgtbl = .);
      . += 0x4000;

      /* we also need a user page table*/
      PROVIDE (_user_pgtbl = .);
       . += 0x4000;

      /* level 2 pages 4k * 4*/
      PROVIDE (_K_l2_pgtbl = .);
      . += 0x200000;
      PROVIDE (_U_l2_pgtbl = .);
      . += 0x200000;
    }
    . = ALIGN(4096);
    PROVIDE (__end = .);
}
__bss_size = ( __bss_end - __bss_start) >> 3;