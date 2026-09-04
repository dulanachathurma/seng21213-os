#include <mm.h>

#define HEAP_SIZE (64 * 1024)

static unsigned char heap[HEAP_SIZE];
static unsigned int  heap_top = 0;

typedef struct block {
    size_t        size;
    int           free;
    struct block *next;
} block_t;

static block_t *free_list = 0;

void mm_init(void) {
    heap_top = 0;
    free_list = 0;
}

void *kmalloc(size_t size) {
    block_t *b;

    for (b = free_list; b; b = b->next) {
        if (b->free && b->size >= size) {
            b->free = 0;
            return (void *)(b + 1);
        }
    }

    size_t needed = sizeof(block_t) + size;
    if (heap_top + needed > HEAP_SIZE) {
        return 0;
    }

    b = (block_t *)(heap + heap_top);
    heap_top += needed;

    b->size = size;
    b->free = 0;
    b->next = free_list;
    free_list = b;

    return (void *)(b + 1);
}

void kfree(void *ptr) {
    if (!ptr) return;
    block_t *b = (block_t *)ptr - 1;
    b->free = 1;
}
