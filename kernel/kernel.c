#include <kernel.h>
#include <vga.h>
#include <keyboard.h>
#include <shell.h>
#include <process.h>
#include <thread.h>
#include <sync.h>
#include <mm.h>
#include <syscall.h>

static mutex_t     print_lock;
static semaphore_t sem;

static void int_to_str(int n, char *buf) {
    if (n == 0) { buf[0] = '0'; buf[1] = '\0'; return; }
    int i = 0, tmp = n;
    char rev[12];
    while (tmp > 0) { rev[i++] = '0' + (tmp % 10); tmp /= 10; }
    int j = 0;
    while (i > 0) buf[j++] = rev[--i];
    buf[j] = '\0';
}

static void thread_a(void) {
    for (int i = 1; i <= 5; i++) {
        mutex_lock(&print_lock);
        vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
        vga_write("[Thread A] step ");
        char buf[8];
        int_to_str(i, buf);
        vga_writeline(buf);
        vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
        mutex_unlock(&print_lock);
        semaphore_signal(&sem);
        thread_yield();
    }
    thread_exit();
}

static void thread_b(void) {
    for (int i = 1; i <= 5; i++) {
        semaphore_wait(&sem);
        mutex_lock(&print_lock);
        vga_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
        vga_write("[Thread B] woke on step ");
        char buf[8];
        int_to_str(i, buf);
        vga_writeline(buf);
        vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
        mutex_unlock(&print_lock);
        thread_yield();
    }
    thread_exit();
}

void kernel_main(void) {
    vga_init();
    keyboard_init();
    mm_init();
    process_init();

    mutex_init(&print_lock);
    semaphore_init(&sem, 0);

    vga_writeline("+=================================================+");
    vga_writeline("| SENG21213 - Full Kernel  (Stages 1 + 2 + 3)    |");
    vga_writeline("| University of Kelaniya                          |");
    vga_writeline("+=================================================+");
    vga_writeline("[ OK ] Memory manager initialised");
    vga_writeline("[ OK ] Process table initialised");

    thread_create(thread_a);
    thread_create(thread_b);

    vga_writeline("[ ** ] Starting threads...");

    syscall_init();

    vga_writeline("[ OK ] IDT loaded, timer IRQ armed");
    vga_writeline("[ OK ] Syscall interface ready (INT 0x80)");
    vga_writeline("");

    schedule();

    vga_writeline("");
    vga_writeline("[ OK ] All threads finished. Dropping to shell.");
    vga_writeline("");

    shell_run();
}

void kernel_halt(void) {
    __asm__ volatile ("cli; hlt");
}
