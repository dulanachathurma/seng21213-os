#include <process.h>

static pcb_t process_table[MAX_PROCESSES];
static int   next_pid = 1;
static int   next_tid = 1;

void process_init(void) {
    for (int i = 0; i < MAX_PROCESSES; i++) {
        process_table[i].pid   = 0;
        process_table[i].tid   = 0;
        process_table[i].state = PROCESS_UNUSED;
        process_table[i].esp   = 0;
    }
}

int process_create(void (*entry)(void)) {
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (process_table[i].state == PROCESS_UNUSED) {
            pcb_t *p = &process_table[i];

            p->pid   = next_pid++;
            p->tid   = next_tid++;
            p->state = PROCESS_READY;

            unsigned int *sp = (unsigned int *)(p->stack + STACK_SIZE);
            sp--;
            *sp = (unsigned int)entry;

            p->esp = (unsigned int)sp;

            return p->pid;
        }
    }
    return -1;
}

pcb_t *process_get_table(void) {
    return process_table;
}
