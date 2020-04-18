ENTRY(Reset_Handler)

_minimum_stack_size = 16K;
_minimum_heap_size = 0;

MEMORY
{
    ITC (rwx)         : ORIGIN = 0x00000000, LENGTH = 32K
    DTC (rwx)         : ORIGIN = 0x20000000, LENGTH = 32K
    OCRAM (rwx)       : ORIGIN = 0x20200000, LENGTH = 64K
}

__StackTop = ORIGIN(OCRAM) + LENGTH(OCRAM);
_bootloader_dbl_tap = ORIGIN(ITC) + LENGTH(ITC) - 4;
__RAM_VECTOR_TABLE_SIZE_BYTES = 0;

SECTIONS
{
    .ivt :
    {
        . = ALIGN(4);
        KEEP(* (.boot_hdr.ivt))
        KEEP(* (.boot_hdr.boot_data))
        KEEP(* (.boot_hdr.dcd_data))
        . = ALIGN(4);
    } > DTC

    .interrupts :
    {
        __VECTOR_TABLE = .;
        __VECTOR_RAM = .;

        . = ALIGN(4);
        KEEP(*(.isr_vector))         /* Startup code */
        . = ALIGN(4);
    } > DTC

    .text :
    {
        . = ALIGN(4);
    *(EXCLUDE_FILE(
        *flexspi_nor_flash_ops.o
        *fsl_flexspi.o
    ) .text*)                /* .text* sections (code) */
        *(.rodata*)        /* .rodata* sections (constants, strings, etc.) */
        . = ALIGN(4);
    } > DTC

    .ARM.exidx :
    {
        *(.ARM.exidx*)
        *(.gnu.linkonce.armexidx.*)
        _etext = .;        /* define a global symbol at end of code */
        __etext = .;       /* define a global symbol at end of code */
    } > DTC

    /* used by the startup to initialize data */
    _sidata = .;

    .data : AT (_sidata)
    {
        . = ALIGN(4);
        __data_start__ = .;     /* create a global symbol at data start */

        *(.data*)               /* .data* sections */
        *flexspi_nor_flash_ops.o(.text*)
        *fsl_flexspi.o(.text*)
        . = ALIGN(4);

        __data_end__ = .;       /* define a global symbol at data end */
    } > DTC

    /* Uninitialized data section */
    .bss :
    {
        . = ALIGN(4);
        __bss_start__ = .;

        *(.bss*)
        *(COMMON)

        . = ALIGN(4);
        __bss_end__ = .;
        PROVIDE(end = .);
    } > ITC

    .heap :
    {
        . = ALIGN(8);
        _heap_start = .;    /* define a global symbol at heap start */

        . += _minimum_heap_size;

        __HeapLimit = .;
    } > ITC

    .stack :
    {
        . = ALIGN(8);
        . += _minimum_stack_size;
    } > ITC

    .ARM.attributes 0 : { *(.ARM.attributes) }
}

