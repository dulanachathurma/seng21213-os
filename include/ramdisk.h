#ifndef RAMDISK_H
#define RAMDISK_H

#define RD_SIZE       (1024 * 1024)
#define RD_BLOCK_SIZE 4096
#define RD_BLOCKS     (RD_SIZE / RD_BLOCK_SIZE)

void ramdisk_init(void);
void ramdisk_read(int block, void *buf);
void ramdisk_write(int block, const void *buf);

#endif
