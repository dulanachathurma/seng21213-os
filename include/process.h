#ifndef PROCESS_H
#define PROCESS_H

#define MAX_PROCESSES  8
#define STACK_SIZE     4096

typedef enum {
    PROCESS_UNUSED  = 0,
    PROCESS_READY   = 1,
    PROCESS_RUNNING = 2,
    PROCESS_BLOCKED = 3,
    PROCESS_DEAD    = 4
} process_state_t;

typedef struct {
    int              pid;
    int              tid;
    process_state_t  state;
    unsigned int     esp;
    unsigned char    stack[STACK_SIZE];
} pcb_t;

void   process_init(void);
int    process_create(void (*entry)(void));
pcb_t *process_get_table(void);
void   schedule(void);

extern void switch_to(unsigned int *old_esp, unsigned int new_esp);

#endif
