OUTPUT_FORMAT ("elf32-littlearm", "elf32-bigarm", "elf32-littlearm")
/* Internal Memory Map*/
MEMORY
{
    rom (rx)  : ORIGIN = 0x00000000, LENGTH = 0x00008000
    ram (rwx) : ORIGIN = 0x20000000, LENGTH = 0x00002000
}

_eram = 0x20000000 + 0x00002000;
SECTIONS
{
    .text :
    {
        KEEP(*(.isr_vector))
        *(.text*)
        
        KEEP(*(.init))
        KEEP(*(.fini))
        
        /* .ctors */
        *crtbegin.o(.ctors)
        *crtbegin?.o(.ctors)
        *(EXCLUDE_FILE(*crtend?.o *crtend.o) .ctors)
        *(SORT(.ctors.*))
        *(.ctors)
        
        /* .dtors */
        *crtbegin.o(.dtors)
        *crtbegin?.o(.dtors)
        *(EXCLUDE_FILE(*crtend?.o *crtend.o) .dtors)
        *(SORT(.dtors.*))
        *(.dtors)
        
        *(.rodata*)
        
        KEEP(*(.eh_fram e*))
        . = ALIGN(4);           /* Added to ensure _sidata is on a 4 byte boundary */
    } > rom 
    
    .ARM.extab : 
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    } > rom 
    
    __exidx_start = .;
    .ARM.exidx :
    {
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
    } > rom 
    __exidx_end = .;
    __etext = .;
    
    /* _sidata is used in coide startup code */
    _sidata = __etext;
    
    .data : AT (__etext)
    {
        __data_start__ = .;
        
        /* _sdata is used in coide startup code */
        _sdata = __data_start__;
        
        *(vtable)
        *(.data*)
        
        . = ALIGN(4);
        /* preinit data */
        PROVIDE_HIDDEN (__preinit_array_start = .);
        KEEP(*(.preinit_array))
        PROVIDE_HIDDEN (__preinit_array_end = .);
        
        . = ALIGN(4);
        /* init data */
        PROVIDE_HIDDEN (__init_array_start = .);
        KEEP(*(SORT(.init_array.*)))
        KEEP(*(.init_array))
        PROVIDE_HIDDEN (__init_array_end = .);
        
        . = ALIGN(4);
        /* finit data */
        PROVIDE_HIDDEN (__fini_array_start = .);
        KEEP(*(SORT(.fini_array.*)))
        KEEP(*(.fini_array))
        PROVIDE_HIDDEN (__fini_array_end = .);
        
        KEEP(*(.jcr*))
        . = ALIGN(4);
        /* All data end */
        __data_end__ = .;
        
        /* _edata is used in coide startup code */
        _edata = __data_end__;
    } > ram 

    __ram_start = ORIGIN(ram);  
    __data_size = _edata - _sdata;
    __end_flash = __data_size + __etext; 
    __crc16_checksum = __end_flash;

    .crc_flash : AT (__crc16_checksum)
    {
        SHORT(0xCAFE);
    } > rom
    
    .bss :
    {
        . = ALIGN(4);
        __bss_start__ = .;
        _sbss = __bss_start__;
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        __bss_end__ = .;
        _ebss = __bss_end__;
    } > ram 
        
    .heap (COPY):
    {
        __end__ = .;
        _end = __end__;
        end = __end__;
        *(.heap*)
        __HeapLimit = .;
    } > ram 
    

    .co_stack (NOLOAD):
    {
        . = ALIGN(4);
        *(.stack_start)
        *(.co_stack .co_stack.*)
        *(.stack_end)
    } > ram 
    
    /* Some stack variables, that can be used */
    __StackTop = ORIGIN(ram ) + LENGTH(ram );
    __StackLimit = __StackTop - SIZEOF(.co_stack);
    __StackSize = SIZEOF(.co_stack);
    PROVIDE(__stack = __StackTop);
    
    /* Check if data + heap + stack exceeds ram  limit */
    ASSERT(__StackLimit >= __HeapLimit, "region ram  overflowed with stack")
}
