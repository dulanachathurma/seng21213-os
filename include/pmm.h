#ifndef PMM_H
#define PMM_H

#define PAGE_SIZE    4096
#define PMM_FRAMES   256

void          pmm_init(void);
unsigned int  pmm_alloc(void);
void          pmm_free(unsigned int frame_addr);
unsigned int  pmm_used(void);
unsigned int  pmm_free_count(void);

#endif
