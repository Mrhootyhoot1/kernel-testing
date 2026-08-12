BITS 16

global _start
extern kernel_main

section .text

_start:
    ;map the memory
    xor ebx, ebx
    mov di, memory_map

    .loop:
        mov edx, 0x534D4150
        mov cx, 24
        mov ax, ds
        mov es, ax
        mov eax, 0xE820

        int 0x15;
        jc end
        cmp ebx, 0
        jz end

        add di, 24
        add word [entry_count], 1
        jmp .loop
    end:
        jmp init_protected_mode

init_protected_mode:
    mov eax, cr0
    or eax, 1
    mov cr0, eax

    jmp 0x08:dword protected_mode

memory_map equ 0x8000

entry_count equ 0x7FFE

BITS 32
protected_mode:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov ss, ax

    mov esp, 0x90000
    push word [entry_count]
    push memory_map
    
    call kernel_main

.hang:
    jmp .hang

section .note.GNU-stack noalloc noexec nowrite progbits