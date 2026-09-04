#include <ramdisk.h>
#include <pmm.h>
#include <string.h>

/*
 * Instead of a 1MB static BSS array (which can overlap the stack or confuse
 * the BIOS), we allocate each 4KB block on demand from the PMM.
 * Only the pointer table lives in BSS (256 * 4 = 1 KB).
 */
static unsigned int rd_pages[RD_BLOCKS];   /* physical frame addresses */

void ramdisk_init(void) {
    for (int i = 0; i < RD_BLOCKS; i++) {
        unsigned int page = pmm_alloc();
        if (page) {
            memset((void *)page, 0, RD_BLOCK_SIZE);
        }
        rd_pages[i] = page;
    }
}

void ramdisk_read(int block, void *buf) {
    if (block < 0 || block >= RD_BLOCKS || !rd_pages[block]) return;
    memcpy(buf, (void *)rd_pages[block], RD_BLOCK_SIZE);
}

void ramdisk_write(int block, const void *buf) {
    if (block < 0 || block >= RD_BLOCKS || !rd_pages[block]) return;
    memcpy((void *)rd_pages[block], buf, RD_BLOCK_SIZE);
}
