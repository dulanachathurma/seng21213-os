#include <vga.h>

#define VGA_WIDTH  80
#define VGA_HEIGHT 25
#define VGA_MEMORY ((volatile unsigned short *)0xB8000)

static int terminal_row    = 0;
static int terminal_column = 0;
static unsigned char terminal_color = 0x07;
static volatile unsigned short *terminal_buffer = VGA_MEMORY;

static inline unsigned char vga_entry_color(enum vga_color fg, enum vga_color bg) {
    return fg | bg << 4;
}

static inline unsigned short vga_entry(unsigned char uc, unsigned char color) {
    return (unsigned short)uc | (unsigned short)color << 8;
}

/* Scroll all rows up by one, blank the last row. */
static void vga_scroll(void) {
    for (int y = 1; y < VGA_HEIGHT; y++) {
        for (int x = 0; x < VGA_WIDTH; x++) {
            terminal_buffer[(y - 1) * VGA_WIDTH + x] =
                terminal_buffer[y * VGA_WIDTH + x];
        }
    }
    /* blank last row */
    for (int x = 0; x < VGA_WIDTH; x++) {
        terminal_buffer[(VGA_HEIGHT - 1) * VGA_WIDTH + x] =
            vga_entry(' ', terminal_color);
    }
    terminal_row = VGA_HEIGHT - 1;
}

void vga_init(void) {
    terminal_row    = 0;
    terminal_column = 0;
    terminal_color  = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    vga_clear();
}

void vga_set_color(unsigned char fg, unsigned char bg) {
    terminal_color = vga_entry_color(fg, bg);
}

void vga_clear(void) {
    for (int y = 0; y < VGA_HEIGHT; y++) {
        for (int x = 0; x < VGA_WIDTH; x++) {
            terminal_buffer[y * VGA_WIDTH + x] = vga_entry(' ', terminal_color);
        }
    }
    terminal_row    = 0;
    terminal_column = 0;
}

void vga_putchar(char c) {
    if (c == '\b') {
        /* Backspace: move cursor back and erase */
        if (terminal_column > 0) {
            terminal_column--;
        } else if (terminal_row > 0) {
            terminal_row--;
            terminal_column = VGA_WIDTH - 1;
        }
        terminal_buffer[terminal_row * VGA_WIDTH + terminal_column] =
            vga_entry(' ', terminal_color);
        return;
    }

    if (c == '\n') {
        terminal_column = 0;
        terminal_row++;
        if (terminal_row >= VGA_HEIGHT) {
            vga_scroll();
        }
        return;
    }

    if (c == '\r') {
        terminal_column = 0;
        return;
    }

    terminal_buffer[terminal_row * VGA_WIDTH + terminal_column] =
        vga_entry(c, terminal_color);

    if (++terminal_column >= VGA_WIDTH) {
        terminal_column = 0;
        terminal_row++;
        if (terminal_row >= VGA_HEIGHT) {
            vga_scroll();
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
