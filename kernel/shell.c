#include <shell.h>
#include <vga.h>
#include <keyboard.h>
#include <string.h>
#include <process.h>
#include <mm.h>
#include <syscall.h>

#define MAX_CMD_LEN 128

static void cmd_help(void) {
    vga_writeline("Available commands:");
    vga_writeline("  help     - List all available commands");
    vga_writeline("  clear    - Clear the screen");
    vga_writeline("  echo     - Print text");
    vga_writeline("  version  - Print kernel version");
    vga_writeline("  colour   - Change text colour (fg bg)");
    vga_writeline("  ps       - List running processes");
    vga_writeline("  meminfo  - Show memory allocator status");
    vga_writeline("  yield    - Yield to scheduler via syscall");
    vga_writeline("  halt     - Halt the CPU");
}

static void cmd_clear(void) {
    vga_clear();
}

static void cmd_echo(const char *args) {
    vga_writeline(args);
}

static void cmd_version(void) {
    vga_writeline("SENG21213 Kernel v3.0 (Stage 1+2+3)");
    vga_writeline("  Stage 1: PCB, Round-Robin Scheduler, Context Switch");
    vga_writeline("  Stage 2: Threads, Mutex, Semaphore");
    vga_writeline("  Stage 3: IDT, Timer IRQ, Syscall INT 0x80, kmalloc");
}

static void cmd_halt(void) {
    vga_writeline("Halting system. You can close QEMU.");
    __asm__ volatile ("cli; hlt");
}

static void int_to_str(int n, char *buf) {
    if (n == 0) { buf[0] = '0'; buf[1] = '\0'; return; }
    int i = 0, tmp = n;
    char rev[12];
    while (tmp > 0) { rev[i++] = '0' + (tmp % 10); tmp /= 10; }
    int j = 0;
    while (i > 0) buf[j++] = rev[--i];
    buf[j] = '\0';
}

static const char *state_name(process_state_t s) {
    switch (s) {
        case PROCESS_UNUSED:  return "UNUSED";
        case PROCESS_READY:   return "READY";
        case PROCESS_RUNNING: return "RUNNING";
        case PROCESS_BLOCKED: return "BLOCKED";
        case PROCESS_DEAD:    return "DEAD";
        default:              return "?";
    }
}

static void cmd_ps(void) {
    pcb_t *table = process_get_table();
    char buf[8];
    vga_writeline("PID  TID  STATE");
    vga_writeline("---  ---  -------");
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (table[i].state != PROCESS_UNUSED) {
            int_to_str(table[i].pid, buf);
            vga_write(buf);
            vga_write("    ");
            int_to_str(table[i].tid, buf);
            vga_write(buf);
            vga_write("    ");
            vga_writeline(state_name(table[i].state));
        }
    }
}

static void cmd_meminfo(void) {
    vga_writeline("Heap: 64KB bump allocator, free-list reclaim");
    vga_writeline("Use kmalloc()/kfree() in kernel code.");
}

static void cmd_yield(void) {
    syscall(SYS_YIELD);
    vga_writeline("[ returned from yield ]");
}

void shell_run(void) {
    char buf[MAX_CMD_LEN];
    int idx = 0;

    vga_writeline("+=================================================+");
    vga_writeline("| SENG21213 Shell  (Stage 1 + 2 + 3)             |");
    vga_writeline("| Type 'help' for available commands              |");
    vga_writeline("+=================================================+");
    vga_write("kernel> ");

    while (1) {
        char c = keyboard_getchar();

        if (c == '\n') {
            vga_putchar('\n');
            buf[idx] = '\0';

            if (idx > 0) {
                if (strcmp(buf, "help") == 0) {
                    cmd_help();
                } else if (strcmp(buf, "clear") == 0) {
                    cmd_clear();
                } else if (strcmp(buf, "version") == 0) {
                    cmd_version();
                } else if (strcmp(buf, "halt") == 0) {
                    cmd_halt();
                } else if (strcmp(buf, "ps") == 0) {
                    cmd_ps();
                } else if (strcmp(buf, "meminfo") == 0) {
                    cmd_meminfo();
                } else if (strcmp(buf, "yield") == 0) {
                    cmd_yield();
                } else if (strncmp(buf, "echo ", 5) == 0) {
                    cmd_echo(buf + 5);
                } else {
                    vga_write("Unknown command: ");
                    vga_writeline(buf);
                }
            }

            idx = 0;
            vga_write("kernel> ");
        } else if (c == '\b') {
            if (idx > 0) {
                idx--;
                vga_putchar('\b');
                vga_putchar(' ');
                vga_putchar('\b');
            }
        } else {
            if (idx < MAX_CMD_LEN - 1) {
                buf[idx++] = c;
                vga_putchar(c);
            }
        }
    }
}
