#include <vga.h>

#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_MEMORY ((volatile unsigned short *)0xB8000)

static int terminal_row = 0;
static int terminal_column = 0;
static unsigned char terminal_color = 0x07;
static volatile unsigned short *terminal_buffer = VGA_MEMORY;

static inline unsigned char vga_entry_color(enum vga_color fg, enum vga_color bg) {
    return fg | bg << 4;
}

static inline unsigned short vga_entry(unsigned char uc, unsigned char color) {
    return (unsigned short)uc | (unsigned short)color << 8;
}

void vga_init(void) {
    terminal_row = 0;
    terminal_column = 0;
    terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    vga_clear();
}

void vga_set_color(unsigned char fg, unsigned char bg) {
    terminal_color = vga_entry_color(fg, bg);
}

void vga_clear(void) {
    for (int y = 0; y < VGA_HEIGHT; y++) {
        for (int x = 0; x < VGA_WIDTH; x++) {
            const size_t index = y * VGA_WIDTH + x;
            terminal_buffer[index] = vga_entry(' ', terminal_color);
        }
    }
    terminal_row = 0;
    terminal_column = 0;
}

void vga_putchar(char c) {
    if (c == '\n') {
        terminal_column = 0;
        if (++terminal_row == VGA_HEIGHT) {
            terminal_row = 0;
        }
        return;
    }

    const size_t index = terminal_row * VGA_WIDTH + terminal_column;
    terminal_buffer[index] = vga_entry(c, terminal_color);

    if (++terminal_column == VGA_WIDTH) {
        terminal_column = 0;
        if (++terminal_row == VGA_HEIGHT) {
            terminal_row = 0;
        }
    }
}

void vga_write(const char *str) {
    for (int i = 0; str[i] != '\0'; i++) {
        vga_putchar(str[i]);
    }
}

void vga_writeline(const char *str) {
    vga_write(str);
    vga_putchar('\n');
}
