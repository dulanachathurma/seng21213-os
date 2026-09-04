#ifndef SYNC_H
#define SYNC_H

typedef struct {
    int locked;
} mutex_t;

typedef struct {
    int count;
} semaphore_t;

void mutex_init(mutex_t *m);
void mutex_lock(mutex_t *m);
void mutex_unlock(mutex_t *m);

void semaphore_init(semaphore_t *s, int value);
void semaphore_wait(semaphore_t *s);
void semaphore_signal(semaphore_t *s);

#endif
