ENTRY(_start)

SECTIONS
{
    . = 0x10000;

    _kernel_start = .;

    .text :
    {
        *(.text)
    }
    .rodata :
    {
        *(.rodata)
    }

    .data :
    {
        *(.data)
    }

    .bss :
    {
        *(COMMON)
        *(.bss)
    }

    /DISCARD/ :
    {
        *(.eh_frame)
        *(.comment)
    }

    _kernel_end = .;
}