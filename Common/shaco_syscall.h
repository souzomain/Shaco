#ifndef _SHACO_SYSCALL_H
#define _SHACO_SYSCALL_H
#include <sys/syscall.h>

#if defined(__x86_64__)
long shaco_syscall(long num, ...);
#else
#include <unistd.h>
#define shaco_syscall syscall
#endif
#endif
