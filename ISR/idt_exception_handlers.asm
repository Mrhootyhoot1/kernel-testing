BITS 32
global common_isr_handler
global undefined_interrupt
%macro ISR_NOERR 1
global isr%+%1

isr%+%1:
    cli
    push dword 0
    push dword %1
    jmp common_isr_handler
%endmacro

%macro ISR_ERR 1
global isr%+%1

isr%+%1:
    cli
    push dword %1
    jmp common_isr_handler
%endmacro
;isr 22-31 are not used
;idc about software interrupts. Nobody should ever write a program for this kernel
; If someone does, they are wasting their time. 
extern exception_handler
extern undefined_handler_handler


ISR_NOERR 0
ISR_NOERR 1
ISR_NOERR 2
ISR_NOERR 3
ISR_NOERR 4
ISR_NOERR 5
ISR_NOERR 6
ISR_NOERR 7
ISR_ERR 8
ISR_NOERR 9
ISR_ERR 10
ISR_ERR 11
ISR_ERR 12
ISR_ERR 13
ISR_ERR 14
ISR_NOERR 15
ISR_NOERR 16
ISR_ERR 17
ISR_NOERR 18
ISR_NOERR 19
ISR_NOERR 20
ISR_ERR 21
;skip reserved
ISR_NOERR 28
ISR_ERR 29
ISR_ERR 30
ISR_ERR 31

undefined_interrupt:
    call undefined_handler_handler


common_isr_handler:
    pushad

    push ds
    push es
    push fs
    push gs

    mov ax, 0x10
    mov ds, ax
    mov es, ax

    push esp
    call exception_handler
    add esp, 4

    pop gs
    pop fs
    pop es
    pop ds

    popad
    add esp, 8
    iretd

section .note.GNU-stack noalloc noexec nowrite progbits