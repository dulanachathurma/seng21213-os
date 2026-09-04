#include <ramdisk.h>
#include <string.h>

static unsigned char ramdisk_data[RD_SIZE];

void ramdisk_init(void) {
    memset(ramdisk_data, 0, RD_SIZE);
}

void ramdisk_read(int block, void *buf) {
    if (block < 0 || block >= RD_BLOCKS) return;
    memcpy(buf, &ramdisk_data[block * RD_BLOCK_SIZE], RD_BLOCK_SIZE);
}

void ramdisk_write(int block, const void *buf) {
    if (block < 0 || block >= RD_BLOCKS) return;
    memcpy(&ramdisk_data[block * RD_BLOCK_SIZE], buf, RD_BLOCK_SIZE);
}
