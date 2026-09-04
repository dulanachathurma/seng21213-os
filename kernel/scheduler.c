#include <process.h>

static int current_pid_index = -1;

extern pcb_t *process_get_table(void);

void schedule(void) {
    pcb_t *table = process_get_table();

    int start = (current_pid_index + 1) % MAX_PROCESSES;
    int next  = -1;

    for (int i = 0; i < MAX_PROCESSES; i++) {
        int idx = (start + i) % MAX_PROCESSES;
        if (table[idx].state == PROCESS_READY) {
            next = idx;
            break;
        }
    }

    if (next == -1) {
        return;
    }

    int prev = current_pid_index;
    current_pid_index = next;

    if (prev != -1 && table[prev].state == PROCESS_RUNNING) {
        table[prev].state = PROCESS_READY;
        table[next].state = PROCESS_RUNNING;
        switch_to(&table[prev].esp, table[next].esp);
    } else {
        table[next].state = PROCESS_RUNNING;
        unsigned int dummy = 0;
        switch_to(&dummy, table[next].esp);
    }
}
