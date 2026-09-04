[org 0x7c00]
[bits 16]

KERNEL_LOAD_SEG EQU 0x1000

start:
    cli
    mov ax, 0x0000
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7c00

    ; Load kernel from disk using BIOS INT 0x13 AH=0x42 (LBA Extended Read)
    mov ah, 0x42
    mov si, dap
    int 0x13
    jc disk_error

    ; Switch to 32-bit Protected Mode
    lgdt [gdt_descriptor]
    mov eax, cr0
    or eax, 1
    mov cr0, eax

    jmp 0x08:init_pm

disk_error:
    jmp $

align 4
dap:
    db 0x10             ; Size of DAP (16 bytes)
    db 0                ; Unused
    dw 128              ; Number of sectors to read
    dw 0x0000           ; Buffer offset
    dw KERNEL_LOAD_SEG  ; Buffer segment
    dq 1                ; Starting LBA (Sector 1, 0-indexed = physical sector 2)

[bits 32]
init_pm:
    mov ax, 0x10
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov esp, 0x1FFFF0   ; Safely above 1MB RAM disk in BSS

    ; Jump to kernel entry point at 0x10000
    call 0x10000

    jmp $

; GDT
gdt_start:
    dq 0x0000000000000000       ; Null descriptor
    dq 0x00cf9a000000ffff       ; 32-bit code descriptor (flat, read/exec)
    dq 0x00cf92000000ffff       ; 32-bit data descriptor (flat, read/write)
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

times 510-($-$$) db 0
dw 0xaa55
