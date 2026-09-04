#include <pmm.h>

#define PMM_BASE 0x200000

static unsigned char bitmap[PMM_FRAMES / 8];

static void bitmap_set(int bit) {
    bitmap[bit / 8] |= (1 << (bit % 8));
}

static void bitmap_clear(int bit) {
    bitmap[bit / 8] &= ~(1 << (bit % 8));
}

static int bitmap_test(int bit) {
    return bitmap[bit / 8] & (1 << (bit % 8));
}

void pmm_init(void) {
    for (int i = 0; i < (int)(PMM_FRAMES / 8); i++) {
        bitmap[i] = 0;
    }
}

unsigned int pmm_alloc(void) {
    for (int i = 0; i < PMM_FRAMES; i++) {
        if (!bitmap_test(i)) {
            bitmap_set(i);
            return PMM_BASE + (unsigned int)(i * PAGE_SIZE);
        }
    }
    return 0;
}

void pmm_free(unsigned int frame_addr) {
    int i = (int)((frame_addr - PMM_BASE) / PAGE_SIZE);
    if (i >= 0 && i < PMM_FRAMES) {
        bitmap_clear(i);
    }
}

unsigned int pmm_used(void) {
    unsigned int count = 0;
    for (int i = 0; i < PMM_FRAMES; i++) {
        if (bitmap_test(i)) count++;
    }
    return count;
}

unsigned int pmm_free_count(void) {
    return PMM_FRAMES - pmm_used();
}
