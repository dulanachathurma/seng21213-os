#include <kernel.h>
#include <vga.h>
#include <keyboard.h>
#include <shell.h>
#include <process.h>
#include <thread.h>
#include <sync.h>
#include <mm.h>
#include <pmm.h>
#include <syscall.h>

static mutex_t     print_lock;
static semaphore_t sem;

static void itoa(int n, char *buf) {
    if (n == 0) { buf[0] = '0'; buf[1] = '\0'; return; }
    char tmp[12];
    int i = 0;
    while (n > 0) { tmp[i++] = '0' + (n % 10); n /= 10; }
    int j = 0;
    while (i > 0) buf[j++] = tmp[--i];
    buf[j] = '\0';
}

static void thread_a(void) {
    for (int i = 1; i <= 4; i++) {
        mutex_lock(&print_lock);
        vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
        vga_write("  [Thread A] tick ");
        char b[8]; itoa(i, b);
        vga_writeline(b);
        vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
        mutex_unlock(&print_lock);
        semaphore_signal(&sem);
        thread_yield();
    }
    thread_exit();
}

static void thread_b(void) {
    for (int i = 1; i <= 4; i++) {
        semaphore_wait(&sem);
        mutex_lock(&print_lock);
        vga_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
        vga_write("  [Thread B] ack  ");
        char b[8]; itoa(i, b);
        vga_writeline(b);
        vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
        mutex_unlock(&print_lock);
        thread_yield();
    }
    thread_exit();
}

void kernel_main(void) {
    vga_init();
    keyboard_init();
    pmm_init();
    mm_init();
    process_init();

    vga_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    vga_writeline("+--------------------------------------------------+");
    vga_writeline("|   SENG21213 OS  --  University of Kelaniya       |");
    vga_writeline("|   Stage 1: Process + Scheduler                   |");
    vga_writeline("|   Stage 2: Threads + Mutex + Semaphore           |");
    vga_writeline("|   Stage 3: PMM + Heap Allocator                  |");
    vga_writeline("|   Stage 4: IDT + Timer IRQ + Syscall INT 0x80    |");
    vga_writeline("+--------------------------------------------------+");
    vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    vga_writeline("");

    vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga_writeline("[ OK ] PMM initialised  (256 frames x 4KB = 1MB pool)");
    vga_writeline("[ OK ] Heap initialised (64KB bump + free-list)");
    vga_writeline("[ OK ] Process table ready");
    vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);

    mutex_init(&print_lock);
    semaphore_init(&sem, 0);

    thread_create(thread_a);
    thread_create(thread_b);

    vga_writeline("");
    vga_set_color(VGA_COLOR_LIGHT_BROWN, VGA_COLOR_BLACK);
    vga_writeline("[ ** ] Launching threads (mutex + semaphore demo)");
    vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);

    syscall_init();

    vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga_writeline("[ OK ] IDT loaded, PIC remapped, Timer IRQ armed");
    vga_writeline("[ OK ] Syscall gate ready  (INT 0x80)");
    vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    vga_writeline("");

    schedule();

    vga_writeline("");
    vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga_writeline("[ OK ] All threads completed.");
    vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    vga_writeline("");

    shell_run();
}

void kernel_halt(void) {
    __asm__ volatile ("cli; hlt");
}
