#include "shaco_syscall.h"
#include <stdarg.h>

#if defined(__x86_64__)
long shaco_syscall(long num, ...) {
    va_list args;
    va_start(args, num);
    long result;

    long arg1 = va_arg(args, long);
    long arg2 = va_arg(args, long);
    long arg3 = va_arg(args, long);
    long arg4 = va_arg(args, long);
    long arg5 = va_arg(args, long);
    long arg6 = va_arg(args, long);

    asm volatile("syscall" : "=a" (result)
                 : "a" (num), "D" (arg1), "S" (arg2), "d" (arg3), "rm" (arg4), "r" (arg5), "r" (arg6)
                 : "rcx", "r11", "memory");
    va_end(args);

    return result;
}

#endif
