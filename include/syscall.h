#ifndef SYSCALL_H
#define SYSCALL_H

#define SYS_YIELD   0
#define SYS_EXIT    1
#define SYS_GETPID  2
#define SYS_PRINT   3
#define SYS_MALLOC  4
#define SYS_FREE    5

void syscall_init(void);
int  syscall(int num, unsigned int arg);

void timer_handler(void);
int  syscall_dispatch(int num);

#endif
