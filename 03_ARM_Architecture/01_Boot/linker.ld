ENTRY(_start)

MEMORY
{
	RAM (rwx) : ORIGIN = 0x40000000, LENGTH = 0x12D39DA80
}

SECTIONS
{
	. = ORIGIN(RAM);

	__ram_start = .;

    .text :
    {
        *(.text*)
    } > RAM

    .rodata :
    {
        *(.rodata*)
    } > RAM

    .data :
    {
        *(.data*)
    } > RAM

    .bss :
    {
        *(.bss*)
        *(COMMON)
    } > RAM

    . = ALIGN(16);

    . += 8192;

    _stack_top = .;
}