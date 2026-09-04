#include <sync.h>
#include <process.h>

void mutex_init(mutex_t *m) {
    m->locked = 0;
}

void mutex_lock(mutex_t *m) {
    while (m->locked) {
        schedule();
    }
    m->locked = 1;
}

void mutex_unlock(mutex_t *m) {
    m->locked = 0;
}

void semaphore_init(semaphore_t *s, int value) {
    s->count = value;
}

void semaphore_wait(semaphore_t *s) {
    while (s->count <= 0) {
        schedule();
    }
    s->count--;
}

void semaphore_signal(semaphore_t *s) {
    s->count++;
}
