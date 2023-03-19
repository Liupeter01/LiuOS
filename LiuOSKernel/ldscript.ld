/*OUTPUT(aarch64)*/
/*ENTRY(_start)*/
ENTRY(_start)
SECTIONS
{
    /*在开启MMU之前严禁使用0XFFFF000000000000以上的地址*/
    . = 0XFFFF000000400000;
    _here = .;
    .text.boot : {*(.text.boot)}

    _text_start = . ;    /*代码段起始地址*/
    .text : {*(.text)}
    _text_end = . ;      /*代码段终止地址*/

    _rodata_start = .;   /*只读数据段起始地址*/
    .rodata : {*(.rodata) }
    _rodata_end = . ;   /*只读数据段终止地址*/

    _data_start = . ;    /*数据段起始地址*/
    .data : {*(.data) }
    _data_end = .;      /*数据段终止地址*/

    idmap_pg_dir = . ;
    . += 4096 ;
    swapper_pg_dir = . ;
    . += 4096 ;
    swapper_pg_end = . ;

    . = ALIGN(0x00200000);

    .bss : { *(.bss)  }
}