#ifndef MM_H
#define MM_H

#include <stddef.h>

void         mm_init(void);
void        *kmalloc(size_t size);
void         kfree(void *ptr);
unsigned int mm_used_bytes(void);
unsigned int mm_free_bytes(void);

#endif
