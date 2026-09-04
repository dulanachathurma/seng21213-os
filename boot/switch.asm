[bits 32]
[global switch_to]

switch_to:
    mov eax, [esp + 4]
    mov ecx, [esp + 8]

    push ebp
    push ebx
    push esi
    push edi

    mov [eax], esp

    mov esp, ecx

    pop edi
    pop esi
    pop ebx
    pop ebp

    ret
