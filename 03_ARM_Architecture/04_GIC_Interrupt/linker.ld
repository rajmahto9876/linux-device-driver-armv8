ENTRY(_start)

MEMORY
{
	RAM (rwx) : ORIGIN = 0x40000000, LENGTH = 0x12D39DA80
}

SECTIONS
{
    /* The virt machine in QEMU loads the kernel image at this address by default. */
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

    /* Leaving Space for L0, L1... levels*/
    _stack_top = . +  0x10000;


    __STACK_START__ = _stack_top;
}