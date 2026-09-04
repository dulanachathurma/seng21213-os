#include <fs.h>
#include <ramdisk.h>
#include <string.h>

#define MAX_INODES 128
#define MAX_DIRENTS (RD_BLOCK_SIZE / sizeof(fs_dirent_t))

static fs_super_t super;
static unsigned char block_bitmap[RD_BLOCK_SIZE];
static unsigned char inode_bitmap[RD_BLOCK_SIZE];
static fs_inode_t inodes[MAX_INODES];
static fs_dirent_t root_dir[MAX_DIRENTS];

static void fs_sync_super(void) {
    unsigned char buf[RD_BLOCK_SIZE];
    memset(buf, 0, RD_BLOCK_SIZE);
    memcpy(buf, &super, sizeof(fs_super_t));
    ramdisk_write(0, buf);
}

static void fs_sync_bitmaps(void) {
    ramdisk_write(2, inode_bitmap);
    ramdisk_write(3, block_bitmap);
}

static void fs_sync_inodes(void) {
    unsigned char buf[RD_BLOCK_SIZE * 2];
    memset(buf, 0, sizeof(buf));
    memcpy(buf, inodes, sizeof(inodes));
    ramdisk_write(4, buf);
    ramdisk_write(5, buf + RD_BLOCK_SIZE);
}

static void fs_sync_dir(void) {
    unsigned char buf[RD_BLOCK_SIZE];
    memset(buf, 0, RD_BLOCK_SIZE);
    memcpy(buf, root_dir, sizeof(root_dir));
    ramdisk_write(1, buf);
}

static int alloc_block(void) {
    for (int i = 8; i < RD_BLOCKS; i++) {
        if (!(block_bitmap[i / 8] & (1 << (i % 8)))) {
            block_bitmap[i / 8] |= (1 << (i % 8));
            super.free_blocks--;
            fs_sync_bitmaps();
            fs_sync_super();
            return i;
        }
    }
    return -1;
}

static int alloc_inode(void) {
    for (int i = 1; i < MAX_INODES; i++) {
        if (!(inode_bitmap[i / 8] & (1 << (i % 8)))) {
            inode_bitmap[i / 8] |= (1 << (i % 8));
            super.free_inodes--;
            memset(&inodes[i], 0, sizeof(fs_inode_t));
            fs_sync_bitmaps();
            fs_sync_super();
            return i;
        }
    }
    return -1;
}

void fs_init(void) {
    unsigned char buf[RD_BLOCK_SIZE];
    ramdisk_read(0, buf);
    memcpy(&super, buf, sizeof(fs_super_t));

    if (super.magic != FS_MAGIC) {
        super.magic = FS_MAGIC;
        super.total_blocks = RD_BLOCKS;
        super.total_inodes = MAX_INODES;
        super.free_blocks = RD_BLOCKS - 8;
        super.free_inodes = MAX_INODES - 1;
        
        memset(block_bitmap, 0, RD_BLOCK_SIZE);
        memset(inode_bitmap, 0, RD_BLOCK_SIZE);
        for(int i=0; i<8; i++) block_bitmap[0] |= (1<<i);
        inode_bitmap[0] |= 1;

        memset(inodes, 0, sizeof(inodes));
        memset(root_dir, 0, sizeof(root_dir));

        fs_sync_super();
        fs_sync_bitmaps();
        fs_sync_inodes();
        fs_sync_dir();
    } else {
        ramdisk_read(2, inode_bitmap);
        ramdisk_read(3, block_bitmap);
        unsigned char ibuf[RD_BLOCK_SIZE * 2];
        ramdisk_read(4, ibuf);
        ramdisk_read(5, ibuf + RD_BLOCK_SIZE);
        memcpy(inodes, ibuf, sizeof(inodes));
        ramdisk_read(1, buf);
        memcpy(root_dir, buf, sizeof(root_dir));
    }
}

int fs_create(const char *name) {
    for (int i = 0; i < MAX_DIRENTS; i++) {
        if (root_dir[i].name[0] == '\0') {
            int ino = alloc_inode();
            if (ino < 0) return -1;
            int nlen = strlen(name);
            if (nlen > 27) nlen = 27;
            memcpy(root_dir[i].name, name, nlen);
            root_dir[i].name[nlen] = '\0';
            root_dir[i].inode = ino;
            fs_sync_dir();
            fs_sync_inodes();
            return ino;
        } else if (strcmp(root_dir[i].name, name) == 0) {
            return -1;
        }
    }
    return -1;
}

int fs_open(const char *name) {
    for (int i = 0; i < MAX_DIRENTS; i++) {
        if (root_dir[i].name[0] != '\0' && strcmp(root_dir[i].name, name) == 0) {
            return root_dir[i].inode;
        }
    }
    return -1;
}

int fs_read(int inode, void *buf, unsigned int size, unsigned int offset) {
    if (inode <= 0 || inode >= MAX_INODES) return -1;
    fs_inode_t *ino = &inodes[inode];
    if (offset >= ino->size) return 0;
    if (offset + size > ino->size) size = ino->size - offset;

    unsigned char *ptr = (unsigned char *)buf;
    unsigned int read = 0;
    unsigned char block_buf[RD_BLOCK_SIZE];

    while (size > 0) {
        int blk_idx = offset / RD_BLOCK_SIZE;
        int blk_off = offset % RD_BLOCK_SIZE;
        if (blk_idx >= 8 || ino->blocks[blk_idx] == 0) break;
        
        ramdisk_read(ino->blocks[blk_idx], block_buf);
        unsigned int chunk = RD_BLOCK_SIZE - blk_off;
        if (chunk > size) chunk = size;

        memcpy(ptr, block_buf + blk_off, chunk);
        ptr += chunk;
        offset += chunk;
        size -= chunk;
        read += chunk;
    }
    return read;
}

int fs_write(int inode, const void *buf, unsigned int size, unsigned int offset) {
    if (inode <= 0 || inode >= MAX_INODES) return -1;
    fs_inode_t *ino = &inodes[inode];
    if (offset > ino->size) offset = ino->size;

    const unsigned char *ptr = (const unsigned char *)buf;
    unsigned int written = 0;
    unsigned char block_buf[RD_BLOCK_SIZE];

    while (size > 0) {
        int blk_idx = offset / RD_BLOCK_SIZE;
        int blk_off = offset % RD_BLOCK_SIZE;
        if (blk_idx >= 8) break;
        
        if (ino->blocks[blk_idx] == 0) {
            int nb = alloc_block();
            if (nb < 0) break;
            ino->blocks[blk_idx] = nb;
            fs_sync_inodes();
        }

        ramdisk_read(ino->blocks[blk_idx], block_buf);
        unsigned int chunk = RD_BLOCK_SIZE - blk_off;
        if (chunk > size) chunk = size;

        memcpy(block_buf + blk_off, ptr, chunk);
        ramdisk_write(ino->blocks[blk_idx], block_buf);

        ptr += chunk;
        offset += chunk;
        size -= chunk;
        written += chunk;
    }

    if (offset > ino->size) {
        ino->size = offset;
        fs_sync_inodes();
    }
    return written;
}

int fs_close(int inode) {
    (void)inode;
    return 0;
}

int fs_unlink(const char *name) {
    for (int i = 0; i < MAX_DIRENTS; i++) {
        if (root_dir[i].name[0] != '\0' && strcmp(root_dir[i].name, name) == 0) {
            int ino = root_dir[i].inode;
            root_dir[i].name[0] = '\0';
            root_dir[i].inode = 0;
            fs_sync_dir();

            for (int j = 0; j < 8; j++) {
                if (inodes[ino].blocks[j]) {
                    int b = inodes[ino].blocks[j];
                    block_bitmap[b / 8] &= ~(1 << (b % 8));
                    super.free_blocks++;
                }
            }
            inode_bitmap[ino / 8] &= ~(1 << (ino % 8));
            super.free_inodes++;
            memset(&inodes[ino], 0, sizeof(fs_inode_t));
            
            fs_sync_bitmaps();
            fs_sync_super();
            fs_sync_inodes();
            return 0;
        }
    }
    return -1;
}

extern void vga_writeline(const char*);
extern void vga_write(const char*);

static void fs_itoa(unsigned int n, char *buf) {
    if (n == 0) { buf[0] = '0'; buf[1] = '\0'; return; }
    char tmp[12];
    int i = 0;
    while (n > 0) { tmp[i++] = '0' + (n % 10); n /= 10; }
    int j = 0;
    while (i > 0) buf[j++] = tmp[--i];
    buf[j] = '\0';
}

void fs_list(void) {
    char buf[12];
    vga_writeline("NAME                        SIZE");
    vga_writeline("-------------------------   ----");
    for (int i = 0; i < MAX_DIRENTS; i++) {
        if (root_dir[i].name[0] != '\0') {
            vga_write(root_dir[i].name);
            for(size_t k = strlen(root_dir[i].name); k < 28; k++) vga_write(" ");
            fs_itoa(inodes[root_dir[i].inode].size, buf);
            vga_writeline(buf);
        }
    }
}
