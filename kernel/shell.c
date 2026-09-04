#include <shell.h>
#include <vga.h>
#include <keyboard.h>
#include <string.h>
#include <process.h>
#include <mm.h>
#include <pmm.h>
#include <syscall.h>
#include <thread.h>
#include <sync.h>
#include <fs.h>
#define MAX_CMD_LEN 128

static void itoa(unsigned int n, char *buf) {
    if (n == 0) { buf[0] = '0'; buf[1] = '\0'; return; }
    char tmp[12];
    int i = 0;
    while (n > 0) { tmp[i++] = '0' + (n % 10); n /= 10; }
    int j = 0;
    while (i > 0) buf[j++] = tmp[--i];
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

static void cmd_help(void) {
    vga_writeline("Commands:");
    vga_writeline("  help    - this list");
    vga_writeline("  clear   - clear screen");
    vga_writeline("  version - kernel version & stages");
    vga_writeline("  echo    - echo text  (usage: echo hello)");
    vga_writeline("  colour  - set text colour  (usage: colour <fg> <bg>)");
    vga_writeline("           colours: 0=black 2=green 3=cyan 4=red");
    vga_writeline("                    7=white 9=lt-blue 10=lt-green");
    vga_writeline("  ps      - list all processes");
    vga_writeline("  memstat - heap and page-frame statistics");
    vga_writeline("  run     - spawn a demo background thread");
    vga_writeline("  yield   - yield CPU via syscall");
    vga_writeline("  ls      - list files in ramdisk");
    vga_writeline("  touch   - create empty file  (usage: touch <name>)");
    vga_writeline("  cat     - read file          (usage: cat <name>)");
    vga_writeline("  write   - append to file     (usage: write <name> <data>)");
    vga_writeline("  rm      - remove file        (usage: rm <name>)");
    vga_writeline("  halt    - halt the CPU");
}

static void cmd_clear(void) {
    vga_clear();
}

static void cmd_echo(const char *args) {
    vga_writeline(args);
}

static void cmd_version(void) {
    vga_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    vga_writeline("SENG21213 OS  v4.0  --  University of Kelaniya");
    vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    vga_writeline("  Stage 1: PCB, round-robin scheduler, context switch");
    vga_writeline("  Stage 2: Threads, mutex, semaphore");
    vga_writeline("  Stage 3: Physical page-frame manager, heap allocator");
    vga_writeline("  Stage 4: IDT, PIC, timer IRQ, syscall, RAM disk FS");
}

static void cmd_halt(void) {
    vga_writeline("System halted. Close QEMU.");
    __asm__ volatile ("cli; hlt");
}

static int parse_colour(const char *s) {
    if (!s || !*s) return -1;
    int n = 0;
    while (*s >= '0' && *s <= '9') { n = n * 10 + (*s - '0'); s++; }
    return (n >= 0 && n <= 15) ? n : -1;
}

static void cmd_colour(const char *args) {
    int fg = -1, bg = -1;
    fg = parse_colour(args);
    while (*args && *args != ' ') args++;
    while (*args == ' ') args++;
    bg = parse_colour(args);

    if (fg < 0 || bg < 0) {
        vga_writeline("Usage: colour <fg 0-15> <bg 0-15>");
        return;
    }
    vga_set_color((unsigned char)fg, (unsigned char)bg);
    vga_writeline("Colour changed.");
}

static void cmd_ps(void) {
    pcb_t *table = process_get_table();
    char buf[8];
    vga_writeline("PID  TID  STATE");
    vga_writeline("---  ---  -------");
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (table[i].state != PROCESS_UNUSED) {
            itoa((unsigned int)table[i].pid, buf);
            vga_write(buf);
            vga_write("    ");
            itoa((unsigned int)table[i].tid, buf);
            vga_write(buf);
            vga_write("    ");
            vga_writeline(state_name(table[i].state));
        }
    }
}

static void cmd_memstat(void) {
    char buf[16];

    vga_writeline("-- Heap Allocator --");
    vga_write("  Used : ");
    itoa(mm_used_bytes(), buf);
    vga_write(buf);
    vga_writeline(" bytes");
    vga_write("  Free : ");
    itoa(mm_free_bytes(), buf);
    vga_write(buf);
    vga_writeline(" bytes");

    vga_writeline("-- Page Frame Manager --");
    vga_write("  Used frames : ");
    itoa(pmm_used(), buf);
    vga_writeline(buf);
    vga_write("  Free frames : ");
    itoa(pmm_free_count(), buf);
    vga_writeline(buf);
    vga_write("  Frame size  : ");
    itoa(PAGE_SIZE, buf);
    vga_write(buf);
    vga_writeline(" bytes");
}

static void demo_thread(void) {
    mutex_t m;
    mutex_init(&m);
    mutex_lock(&m);
    vga_set_color(VGA_COLOR_LIGHT_MAGENTA, VGA_COLOR_BLACK);
    vga_writeline("  [demo] thread running via mutex");
    vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    mutex_unlock(&m);
    thread_exit();
}

static void cmd_run(void) {
    int pid = thread_create(demo_thread);
    char buf[8];
    if (pid < 0) {
        vga_writeline("Process table full.");
    } else {
        vga_write("Thread spawned with PID ");
        itoa((unsigned int)pid, buf);
        vga_writeline(buf);
        schedule();
    }
}

static void cmd_yield(void) {
    syscall(SYS_YIELD, 0);
    vga_writeline("[ returned from yield ]");
}

static void cmd_ls(void) {
    fs_list();
}

static void cmd_touch(const char *name) {
    if (!name || !*name) {
        vga_writeline("Usage: touch <name>");
        return;
    }
    if (fs_create(name) < 0) {
        vga_writeline("Failed to create file.");
    } else {
        vga_writeline("File created.");
    }
}

static void cmd_cat(const char *name) {
    if (!name || !*name) {
        vga_writeline("Usage: cat <name>");
        return;
    }
    int inode = fs_open(name);
    if (inode < 0) {
        vga_writeline("File not found.");
        return;
    }
    char buf[256];
    int read = fs_read(inode, buf, sizeof(buf) - 1, 0);
    if (read < 0) {
        vga_writeline("Read error.");
    } else {
        buf[read] = '\0';
        vga_writeline(buf);
    }
    fs_close(inode);
}

static void cmd_write(const char *args) {
    if (!args || !*args) {
        vga_writeline("Usage: write <name> <data>");
        return;
    }
    char name[32];
    int i = 0;
    while (*args && *args != ' ' && i < 31) {
        name[i++] = *args++;
    }
    name[i] = '\0';
    while (*args == ' ') args++;
    
    if (!*args) {
        vga_writeline("Usage: write <name> <data>");
        return;
    }
    
    int inode = fs_open(name);
    if (inode < 0) {
        vga_writeline("File not found.");
        return;
    }
    int len = strlen(args);
    int w = fs_write(inode, args, len, 0); // append behavior not fully precise without knowing file size, but fs_write does append if offset > size
    if (w < 0) {
        vga_writeline("Write error.");
    } else {
        vga_writeline("Data written.");
    }
    fs_close(inode);
}

static void cmd_rm(const char *name) {
    if (!name || !*name) {
        vga_writeline("Usage: rm <name>");
        return;
    }
    if (fs_unlink(name) < 0) {
        vga_writeline("File not found or error.");
    } else {
        vga_writeline("File removed.");
    }
}

void shell_run(void) {
    char buf[MAX_CMD_LEN];
    int idx = 0;

    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_writeline("+--------------------------------------------------+");
    vga_writeline("|   SENG21213 Shell  --  All Stages Integrated      |");
    vga_writeline("|   Type 'help' for commands                        |");
    vga_writeline("+--------------------------------------------------+");
    vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
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
                } else if (strcmp(buf, "memstat") == 0) {
                    cmd_memstat();
                } else if (strcmp(buf, "run") == 0) {
                    cmd_run();
                } else if (strcmp(buf, "yield") == 0) {
                    cmd_yield();
                } else if (strcmp(buf, "ls") == 0) {
                    cmd_ls();
                } else if (strncmp(buf, "touch ", 6) == 0) {
                    cmd_touch(buf + 6);
                } else if (strncmp(buf, "cat ", 4) == 0) {
                    cmd_cat(buf + 4);
                } else if (strncmp(buf, "write ", 6) == 0) {
                    cmd_write(buf + 6);
                } else if (strncmp(buf, "rm ", 3) == 0) {
                    cmd_rm(buf + 3);
                } else if (strncmp(buf, "echo ", 5) == 0) {
                    cmd_echo(buf + 5);
                } else if (strncmp(buf, "colour ", 7) == 0) {
                    cmd_colour(buf + 7);
                } else {
                    vga_write("Unknown: ");
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
