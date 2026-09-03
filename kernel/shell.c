#include <shell.h>
#include <vga.h>
#include <keyboard.h>
#include <string.h>

#define MAX_CMD_LEN 128

static void cmd_help(void) {
    vga_writeline("Available commands:");
    vga_writeline("  help     - List all available commands");
    vga_writeline("  clear    - Clear the screen");
    vga_writeline("  echo     - Print text");
    vga_writeline("  version  - Print kernel version");
    vga_writeline("  colour   - Change text colour (fg bg)");
    vga_writeline("  halt     - Halt the CPU");
}

static void cmd_clear(void) {
    vga_clear();
}

static void cmd_echo(const char *args) {
    vga_writeline(args);
}

static void cmd_version(void) {
    vga_writeline("SENG21213 Stage 0 Kernel v1.0");
}

static void cmd_halt(void) {
    vga_writeline("Haltering system. You can close QEMU.");
    __asm__ volatile ("cli; hlt");
}

void shell_run(void) {
    char buf[MAX_CMD_LEN];
    int idx = 0;

    vga_writeline("+=================================================+");
    vga_writeline("| SENG 21213 - Stage 0 Kernel Shell               |");
    vga_writeline("| University of Kelaniya                          |");
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
                } else if (__builtin_strncmp(buf, "echo ", 5) == 0) {
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
