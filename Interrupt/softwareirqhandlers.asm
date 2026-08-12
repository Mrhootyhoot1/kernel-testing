BITS 32
global int80
extern software_interrupt_handler

%macro INTERRUPT 1
global int%+%1

int%+%1:
    push ds
    push es
    push fs
    push gs

    pusha

    push dword %1
    push esp
    call software_interrupt_handler
    add esp, 8

    mov esp, eax

    popa
    
    pop gs
    pop fs
    pop es
    pop ds
    iretd
%endmacro

INTERRUPT 0x80