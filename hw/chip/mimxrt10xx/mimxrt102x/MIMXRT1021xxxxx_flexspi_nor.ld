ENTRY(Reset_Handler)

_minimum_stack_size = 12K;
_minimum_heap_size = 0;

MEMORY
{
    FLASH_CONFIG (rx) : ORIGIN = 0x60000000, LENGTH = 2K
    FLASH_IVT (rx)    : ORIGIN = 0x60001000, LENGTH = 4K
    FLASH_ISR (rx)    : ORIGIN = 0x60002000, LENGTH = 1K
    FLASH_TEXT (rx)   : ORIGIN = 0x60002400, LENGTH = 27K
    OCRAM (rwx)       : ORIGIN = 0x20200000, LENGTH = 32K
}

__StackTop = ORIGIN(OCRAM) + LENGTH(OCRAM) - 8;
_bootloader_dbl_tap = ORIGIN(OCRAM) + LENGTH(OCRAM) - 4;
__RAM_VECTOR_TABLE_SIZE_BYTES = 0;

SECTIONS
{
    .flash_config :
    {
        . = ALIGN(4);
        __FLASH_BASE = .;        
        KEEP(* (.boot_hdr.conf))
        . = ALIGN(4);
    } > FLASH_CONFIG

    .ivt :
    {
        . = ALIGN(4);
        KEEP(* (.boot_hdr.ivt))
        KEEP(* (.boot_hdr.boot_data))
        KEEP(* (.boot_hdr.dcd_data))
        . = ALIGN(4);
    } > FLASH_IVT

    .interrupts :
    {
        __VECTOR_TABLE = .;
        __VECTOR_RAM = .;

        . = ALIGN(4);
        KEEP(*(.isr_vector))         /* Startup code */
        . = ALIGN(4);
    } > FLASH_ISR

    .text :
    {
        . = ALIGN(4);
    *(EXCLUDE_FILE(
        *flexspi_nor_flash_ops.o
        *fsl_flexspi.o
    ) .text*)                /* .text* sections (code) */
        *(.rodata*)        /* .rodata* sections (constants, strings, etc.) */
        . = ALIGN(4);
    } > FLASH_TEXT

    .ARM.exidx :
    {
        *(.ARM.exidx*)
        *(.gnu.linkonce.armexidx.*)
        _etext = .;        /* define a global symbol at end of code */
        __etext = .;       /* define a global symbol at end of code */
    } > FLASH_TEXT

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
    } > OCRAM

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
    } > OCRAM

    .heap :
    {
        . = ALIGN(8);
        _heap_start = .;    /* define a global symbol at heap start */

        . += _minimum_heap_size;

        __HeapLimit = .;
    } > OCRAM

    .stack :
    {
        . = ALIGN(8);
        . += _minimum_stack_size;
    } > OCRAM

    .ARM.attributes 0 : { *(.ARM.attributes) }
}
