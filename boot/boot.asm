[org 0x7c00]
[bits 16]

start:
    ; print CrunchOS
    mov ah, 0x0e
    mov al, 'C'
    int 0x10
    mov al, 'r'
    int 0x10
    mov al, 'u'
    int 0x10
    mov al, 'n'
    int 0x10
    mov al, 'c'
    int 0x10
    mov al, 'h'
    int 0x10
    mov al, 'O'
    int 0x10
    mov al, 'S'
    int 0x10

    ; load kernel from disk
    mov bx, 0x1000    ; load kernel to memory address 0x1000
    mov dh, 1         ; read 1 sector
    mov dl, 0x80      ; from hard disk
    call disk_load

    ; jump to kernel
    jmp 0x1000

disk_load:
    mov ah, 0x02
    mov al, dh
    mov ch, 0x00
    mov cl, 0x02
    mov dh, 0x00
    int 0x13
    ret

times 510-($-$$) db 0
dw 0xAA55