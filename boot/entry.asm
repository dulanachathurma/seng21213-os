[bits 32]
[global _start]
[extern kernel_main]

; This is the very first code linked into the kernel binary.
; The linker script places this at 0x10000.
; The bootloader does 'call 0x10000' which lands here.
_start:
    call kernel_main
    cli
    hlt
