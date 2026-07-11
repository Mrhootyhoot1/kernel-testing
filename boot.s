BITS 16
ORG 0x7C00

global start

start:
    cli
    mov [boot_drive], dl
    jmp read_kernel ;dont execute the GDT

read_kernel:
; Set destination ES:BX = 0x10000
    mov ax, 0x1000
    mov es, ax
    xor bx, bx

    mov ah, 0x02
    mov al, 16       ; Read 16 sectors
    mov ch, 0       ; Cylinder 0
    mov cl, 2       ; Sector 2
    mov dh, 0       ; Head 0
    mov dl, [boot_drive]

    int 0x13

    jc disk_error
    
    jmp setup

disk_error:
    mov ah, 0x0E
    mov al, 'E'
    int 0x10
    cli
    .loop:
        hlt
        jmp .loop

gdt_start:
    dq 0x0000000000000000 ;null

    dq 0x00CF9A000000FFFF   ; codee
    dq 0x00CF92000000FFFF   ; data 
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

boot_drive:
    boot_drive db 0

setup:
    push ax
    mov ax, 0x2401  ; Enable a20
    int 0x15
    pop ax
    lgdt [gdt_descriptor]
    jmp 0x1000:0x0000
times 510-($-$$) db 0
dw 0xAA55
