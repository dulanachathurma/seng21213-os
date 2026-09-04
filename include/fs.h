#ifndef FS_H
#define FS_H

#define FS_MAGIC 0x53454E47

typedef struct {
    unsigned int magic;
    unsigned int total_blocks;
    unsigned int total_inodes;
    unsigned int free_blocks;
    unsigned int free_inodes;
} fs_super_t;

typedef struct {
    unsigned int size;
    unsigned int blocks[8];
} fs_inode_t;

typedef struct {
    char name[28];
    unsigned int inode;
} fs_dirent_t;

void fs_init(void);
int fs_create(const char *name);
int fs_open(const char *name);
int fs_read(int inode, void *buf, unsigned int size, unsigned int offset);
int fs_write(int inode, const void *buf, unsigned int size, unsigned int offset);
int fs_close(int inode);
int fs_unlink(const char *name);
void fs_list(void);

#endif
