ENTRY(_start)
SECTIONS
{
    . = 0x10000;

    .text : {
        build/entry.o(.text)   /* entry stub must be first: bootloader calls 0x10000 */
        *(.text)
    }
    .rodata : { *(.rodata) }
    .data   : { *(.data) }
    .bss    : { *(.bss) *(COMMON) }
}
