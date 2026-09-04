[org 0x7c00]
[bits 16]

start:
    cli
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7c00

    ; Read 32 sectors from disk (16 KB) starting at sector 2 into 0x1000:0x0000 = 0x10000
    mov ah, 0x02
    mov al, 32          ; 32 sectors (well within one track, kernel is only 26 sectors)
    mov ch, 0           ; cylinder 0
    mov cl, 2           ; sector 2 (boot sector is sector 1)
    mov dh, 0           ; head 0
    ; dl = boot drive, BIOS already sets this on entry - do NOT clobber it
    mov bx, 0x1000
    mov es, bx
    xor bx, bx          ; ES:BX = 0x1000:0x0000 = physical 0x10000
    int 0x13
    jc disk_error

    ; Load GDT and enter 32-bit protected mode
    lgdt [gdt_descriptor]
    mov eax, cr0
    or  eax, 1
    mov cr0, eax
    jmp 0x08:protected_mode

disk_error:
    jmp $

[bits 32]
protected_mode:
    ; Set all data segments to the flat 32-bit data descriptor
    mov ax, 0x10
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov esp, 0x90000    ; stack below BIOS region (0xA0000), above kernel (~0x15000)

    call 0x10000        ; call kernel_main at physical address 0x10000

    cli
    hlt                 ; should never reach here

; ---- GDT ----
gdt_start:
    dq 0x0000000000000000       ; null descriptor
    dq 0x00cf9a000000ffff       ; 0x08: 32-bit code, base=0, limit=4GB, R/X
    dq 0x00cf92000000ffff       ; 0x10: 32-bit data, base=0, limit=4GB, R/W
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1  ; limit
    dd gdt_start                 ; base (physical address of gdt_start)

times 510-($-$$) db 0
dw 0xAA55
