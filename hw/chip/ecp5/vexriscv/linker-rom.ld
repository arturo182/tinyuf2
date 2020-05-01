INCLUDE generated/output_format.ld
ENTRY(_start)

__DYNAMIC = 0;

INCLUDE generated/regions.ld

SECTIONS
{
	.text :
	{
		_ftext = .;
		*(.text .stub .text.* .gnu.linkonce.t.*)
		_etext = .;
	} > rom

	.got :
	{
		_GLOBAL_OFFSET_TABLE_ = .;
		*(.got)
	} > rom

	.got.plt :
	{
		*(.got.plt)
	} > rom

	.rodata :
	{
		. = ALIGN(4);
		_frodata = .;
		*(.rodata .rodata.* .gnu.linkonce.r.*)
		*(.rodata1)
                . = ALIGN(4);
		_erodata = .;
                _sidata = _erodata;
	} > rom

	.data : AT ( _sidata )
	{
		. = ALIGN(4);
		_sdata = .;
		*(.data .data.* .gnu.linkonce.d.*)
		*(.data1)
		*(.sdata .sdata* .eh_frame* .gnu.linkonce.s.*)
		_edata = .;
	} > sram

	.bss :
	{
		. = ALIGN(4);
		_fbss = .;
		*(.dynsbss)
		*(.sbss .sbss.* .gnu.linkonce.sb.*)
		*(.scommon)
		*(.dynbss)
		*(.bss .bss.* .gnu.linkonce.b.*)
		*(COMMON)
		. = ALIGN(4);
		_ebss = .;
		. = ALIGN(8);
		_heapstart = .;
	} > sram

        .flash :
        {
                _fstart = .;
                _fend = .;
        } > spiflash

}

PROVIDE(_fstack = ORIGIN(sram) + LENGTH(sram) - 4);
PROVIDE(_flash_start = _fstart);
