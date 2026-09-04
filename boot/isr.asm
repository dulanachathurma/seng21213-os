[bits 32]
[global idt_load]
[global isr_timer]
[global isr_syscall]
[extern timer_handler]
[extern syscall_handler]

idt_load:
    mov eax, [esp + 4]
    lidt [eax]
    ret

isr_timer:
    pusha
    call timer_handler
    mov al, 0x20
    out 0x20, al
    popa
    iret

isr_syscall:
    pusha
    push eax
    call syscall_handler
    add esp, 4
    popa
    iret
