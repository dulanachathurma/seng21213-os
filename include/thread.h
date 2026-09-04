#ifndef THREAD_H
#define THREAD_H

#include <process.h>

int  thread_create(void (*entry)(void));
void thread_exit(void);
void thread_yield(void);

#endif
