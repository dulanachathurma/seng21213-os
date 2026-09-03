#include <keyboard.h>
#include <io.h>

#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64

// US QWERTY Scancode Set 1 (Press events only for basic stage 0)
static unsigned char scancode_ascii[128] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
    '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, '-', 0, 0, 0, '+', 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

void keyboard_init(void) {
    // Clear any residual data in the buffer
    while (inb(KEYBOARD_STATUS_PORT) & 1) {
        inb(KEYBOARD_DATA_PORT);
    }
}

char keyboard_getchar(void) {
    while (1) {
        // Wait until output buffer is full (key pressed)
        if (inb(KEYBOARD_STATUS_PORT) & 1) {
            unsigned char scancode = inb(KEYBOARD_DATA_PORT);
            // Ignore key release events (high bit set)
            if (!(scancode & 0x80)) {
                if (scancode < 128) {
                    char c = scancode_ascii[scancode];
                    if (c != 0) {
                        return c;
                    }
                }
            }
        }
    }
}
