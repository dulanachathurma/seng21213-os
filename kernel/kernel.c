#include <kernel.h>
#include <vga.h>
#include <keyboard.h>
#include <shell.h>

void kernel_main(void) {
    vga_init();
    keyboard_init();

    vga_writeline("SENG21213 Stage 0 -- Loading kernel...");
    vga_writeline("Kernel loaded. Switching to Protected Mode.");
    vga_writeline("[ OK ] VGA driver initialised");
    vga_writeline("[ OK ] PS/2 keyboard driver initialised");

    shell_run();
}

void kernel_halt(void) {
    __asm__ volatile ("cli; hlt");
}
