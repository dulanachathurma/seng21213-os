#include <kernel.h>
#include <vga.h>
#include <keyboard.h>
#include <shell.h>
#include <process.h>
#include <mm.h>
#include <pmm.h>
#include <syscall.h>
#include <ramdisk.h>
#include <fs.h>

void kernel_main(void) {
    vga_init();
    keyboard_init();
    pmm_init();
    mm_init();
    process_init();
    ramdisk_init();
    fs_init();
    syscall_init();

    vga_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    vga_writeline("+--------------------------------------------------+");
    vga_writeline("|   SENG21213 OS  --  University of Kelaniya       |");
    vga_writeline("|   Stage 1: Process + Scheduler                   |");
    vga_writeline("|   Stage 2: Threads + Mutex + Semaphore           |");
    vga_writeline("|   Stage 3: PMM + Heap Allocator                  |");
    vga_writeline("|   Stage 4: RAM Disk + File System                |");
    vga_writeline("+--------------------------------------------------+");
    vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    vga_writeline("");

    vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga_writeline("[ OK ] VGA + Keyboard ready");
    vga_writeline("[ OK ] PMM initialised  (256 frames x 4KB = 1MB pool)");
    vga_writeline("[ OK ] Heap initialised (64KB bump allocator)");
    vga_writeline("[ OK ] Process table ready");
    vga_writeline("[ OK ] RAM Disk & File System initialised");
    vga_writeline("[ OK ] IDT loaded, PIC remapped, syscall gate ready");
    vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    vga_writeline("");

    shell_run();
}

void kernel_halt(void) {
    __asm__ volatile ("cli; hlt");
}
