[bits 32]
[global idt_load]
[global isr_timer]
[global isr_syscall]
[extern timer_handler]
[extern syscall_dispatch]

idt_load:
    mov eax, [esp + 4]
    lidt [eax]
    ret

isr_timer:
    pusha
    push ds
    push es
    push fs
    push gs
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    call timer_handler
    mov al, 0x20
    out 0x20, al
    pop gs
    pop fs
    pop es
    pop ds
    popa
    iret

isr_syscall:
    pusha
    push ds
    push es
    push fs
    push gs
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    push eax
    call syscall_dispatch
    add esp, 4
    pop gs
    pop fs
    pop es
    pop ds
    popa
    iret
