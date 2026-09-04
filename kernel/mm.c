#include <mm.h>
#include <pmm.h>

#define HEAP_SIZE  (64 * 1024)

typedef struct block {
    size_t        size;
    int           free;
    struct block *next;
} block_t;

static unsigned char  heap[HEAP_SIZE];
static unsigned int   heap_top   = 0;
static unsigned int   heap_used  = 0;
static block_t       *free_list  = 0;

void mm_init(void) {
    heap_top  = 0;
    heap_used = 0;
    free_list = 0;
}

void *kmalloc(size_t size) {
    block_t *b;

    for (b = free_list; b; b = b->next) {
        if (b->free && b->size >= size) {
            b->free = 0;
            heap_used += b->size;
            return (void *)(b + 1);
        }
    }

    size_t needed = sizeof(block_t) + size;
    if (heap_top + needed > HEAP_SIZE) {
        return 0;
    }

    b = (block_t *)(heap + heap_top);
    heap_top += (unsigned int)needed;

    b->size = size;
    b->free = 0;
    b->next = free_list;
    free_list = b;

    heap_used += size;
    return (void *)(b + 1);
}

void kfree(void *ptr) {
    if (!ptr) return;
    block_t *b = (block_t *)ptr - 1;
    if (!b->free) {
        b->free = 1;
        heap_used -= b->size;
    }
}

unsigned int mm_used_bytes(void) {
    return heap_used;
}

unsigned int mm_free_bytes(void) {
    return HEAP_SIZE - heap_top;
}
