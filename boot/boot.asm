[org 0x7C00]
[bits 16]

start:
    mov ah, 0x0E

print:
    mov si, message

.loop:
    lodsb
    cmp al, 0
    je load_kernel

    int 0x10
    jmp .loop

load_kernel:
    mov ah, 0x02
    mov al, 4
    mov ch, 0
    mov cl, 2
    mov dh, 0
    mov bx, 0x1000
    int 0x13

    jmp 0x0000:0x1000

message db 'CrunchOS', 0

times 510-($-$$) db 0
dw 0xAA55
