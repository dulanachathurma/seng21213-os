#ifndef SYSCALL_H
#define SYSCALL_H

#define SYS_YIELD  0
#define SYS_EXIT   1
#define SYS_GETPID 2

void syscall_init(void);
int  syscall(int num);

#endif
