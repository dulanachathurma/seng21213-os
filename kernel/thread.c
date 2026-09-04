#include <thread.h>
#include <process.h>

int thread_create(void (*entry)(void)) {
    return process_create(entry);
}

void thread_exit(void) {
    pcb_t *table = process_get_table();

    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (table[i].state == PROCESS_RUNNING) {
            table[i].state = PROCESS_DEAD;
            break;
        }
    }

    schedule();
}

void thread_yield(void) {
    schedule();
}
