#include "syscall.h"
#include <sys/syscall.h>
#include <stdarg.h>

long shaco_syscall(long num, ...) {
    va_list args;
    va_start(args, num);

    long result;
#if defined(__x86_64__)
    long arg1 = va_arg(args, long);
    long arg2 = va_arg(args, long);
    long arg3 = va_arg(args, long);
    long arg4 = va_arg(args, long);
    long arg5 = va_arg(args, long);
    long arg6 = va_arg(args, long);

    asm volatile("syscall" : "=a" (result)
                 : "a" (num), "D" (arg1), "S" (arg2), "d" (arg3), "rm" (arg4), "r" (arg5), "r" (arg6)
                 : "rcx", "r11", "memory");
#elif defined(__i386__)
    long arg1 = va_arg(args, long);
    long arg2 = va_arg(args, long);
    long arg3 = va_arg(args, long);
    long arg4 = va_arg(args, long);

    asm volatile("int $0x80" : "=a" (result)
                 : "a" (num), "b" (arg1), "c" (arg2), "d" (arg3), "S" (arg4)
                 : "memory");
#else
    return syscall(num, va_arg(args, long), va_arg(args, long), va_arg(args, long), va_arg(args, long), va_arg(args, long), va_arg(args, long));
#endif

    va_end(args);

    return result;
}
