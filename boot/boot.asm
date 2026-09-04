[org 0x7c00]
[bits 16]

KERNEL_LOAD_SEG EQU 0x1000

start:
    cli
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7c00

    ; BIOS puts boot drive number in DL - save it
    mov [boot_drive], dl

    ; Probe for LBA extensions (INT 13h AH=41h)
    mov ah, 0x41
    mov bx, 0x55AA
    mov dl, [boot_drive]
    int 0x13
    jc  .use_chs
    cmp bx, 0xAA55
    jne .use_chs

    ; ---- LBA path: single call reads 128 sectors ----
    mov ah, 0x42
    mov dl, [boot_drive]
    mov si, dap
    int 0x13
    jnc .done
    jmp disk_error

.use_chs:
    ; ---- CHS fallback: 8 calls x 16 sectors = 128 sectors ----
    mov ax, KERNEL_LOAD_SEG
    mov es, ax
    xor bx, bx
    mov [chs_sec],  byte 2       ; first sector (1-based)
    mov [chs_iter], byte 8       ; number of iterations

.chs_loop:
    mov ah, 0x02
    mov al, 16                   ; 16 sectors per call
    mov ch, 0
    mov dh, 0
    mov cl, [chs_sec]
    mov dl, [boot_drive]
    int 0x13
    jc  disk_error

    add bx, 0x2000               ; advance buffer 8 KB
    jnc .no_wrap
    ; BX wrapped: advance segment by 0x800 (= 8 KB / 16)
    mov ax, es
    add ax, 0x0800
    mov es, ax
.no_wrap:
    add byte [chs_sec], 16
    dec byte [chs_iter]
    jnz .chs_loop

.done:
    ; Enter 32-bit protected mode
    lgdt [gdt_descriptor]
    mov eax, cr0
    or  eax, 1
    mov cr0, eax
    jmp 0x08:init_pm

disk_error:
    mov ah, 0x0E
    mov al, 'E'
    int 0x10
    jmp $

; ---- variables (in 16-bit data area of boot sector) ----
boot_drive: db 0
chs_sec:    db 0
chs_iter:   db 0

align 4
dap:
    db 0x10             ; DAP structure size
    db 0x00             ; reserved
    dw 128              ; sectors to read
    dw 0x0000           ; destination offset
    dw KERNEL_LOAD_SEG  ; destination segment  -> physical 0x10000
    dq 1                ; LBA start (sector 1 = physical sector 2)

[bits 32]
init_pm:
    mov ax, 0x10
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov esp, 0x90000    ; safe stack, below BIOS region (0xA0000)

    call 0x10000        ; jump to kernel_main

    jmp $

; ---- GDT ----
gdt_start:
    dq 0x0000000000000000        ; null
    dq 0x00cf9a000000ffff        ; 32-bit code
    dq 0x00cf92000000ffff        ; 32-bit data
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

times 510-($-$$) db 0
dw 0xAA55
