BITS 32
global generic_irq_handler
extern irq_handler

%macro IRQ_HANDLER 1
global irq%+%1

irq%+%1:
    push dword 0
    push dword %1
    jmp generic_irq_handler
%endmacro

section .data

section .text

IRQ_HANDLER 32
IRQ_HANDLER 33
IRQ_HANDLER 34
IRQ_HANDLER 35
IRQ_HANDLER 36
IRQ_HANDLER 37
IRQ_HANDLER 38
IRQ_HANDLER 39
IRQ_HANDLER 40
IRQ_HANDLER 41
IRQ_HANDLER 42
IRQ_HANDLER 43
IRQ_HANDLER 44
IRQ_HANDLER 45
IRQ_HANDLER 46
IRQ_HANDLER 47

generic_irq_handler:
    pushad

    push ds
    push es
    push fs
    push gs

    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    push esp
    call irq_handler
    add esp, 4

    pop gs
    pop fs
    pop es
    pop ds

    popad

    add esp, 8       ; remove irq + error
    iretd