#include <syscall.h>
#include <process.h>
#include <io.h>

#define IDT_SIZE 256

typedef struct {
    unsigned short offset_low;
    unsigned short selector;
    unsigned char  zero;
    unsigned char  type_attr;
    unsigned short offset_high;
} __attribute__((packed)) idt_entry_t;

typedef struct {
    unsigned short limit;
    unsigned int   base;
} __attribute__((packed)) idt_ptr_t;

static idt_entry_t idt[IDT_SIZE];
static idt_ptr_t   idt_ptr;

extern void idt_load(idt_ptr_t *ptr);
extern void isr_timer(void);
extern void isr_syscall(void);

static void idt_set(int n, void (*handler)(void)) {
    unsigned int addr = (unsigned int)handler;
    idt[n].offset_low  = addr & 0xFFFF;
    idt[n].selector    = 0x08;
    idt[n].zero        = 0;
    idt[n].type_attr   = 0x8E;
    idt[n].offset_high = (addr >> 16) & 0xFFFF;
}

static void pic_remap(void) {
    outb(0x20, 0x11);
    outb(0xA0, 0x11);
    outb(0x21, 0x20);
    outb(0xA1, 0x28);
    outb(0x21, 0x04);
    outb(0xA1, 0x02);
    outb(0x21, 0x01);
    outb(0xA1, 0x01);
    outb(0x21, 0xFE);
    outb(0xA1, 0xFF);
}

void timer_handler(void) {
    schedule();
}

int syscall_handler(int num) {
    switch (num) {
        case SYS_YIELD:
            schedule();
            return 0;
        case SYS_EXIT:
            return 0;
        default:
            return -1;
    }
}

void syscall_init(void) {
    for (int i = 0; i < IDT_SIZE; i++) {
        idt[i].offset_low  = 0;
        idt[i].selector    = 0;
        idt[i].zero        = 0;
        idt[i].type_attr   = 0;
        idt[i].offset_high = 0;
    }

    pic_remap();

    idt_set(0x20, isr_timer);
    idt_set(0x80, isr_syscall);

    idt_ptr.limit = sizeof(idt) - 1;
    idt_ptr.base  = (unsigned int)&idt;

    idt_load(&idt_ptr);

    __asm__ volatile ("sti");
}

int syscall(int num) {
    int ret;
    __asm__ volatile (
        "int $0x80"
        : "=a"(ret)
        : "a"(num)
    );
    return ret;
}
