#include "Protect.h"

#ifdef ANTIDEBUG
#include <sys/ptrace.h>
#include <sys/mman.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>

#define LINE_SIZE 128

bool check_for_debug_libraries() {
    pid_t pid = getpid();
    char proc_pid_maps[32];
    snprintf(proc_pid_maps, sizeof(proc_pid_maps), "/proc/%d/maps", pid);
    int fd = open(proc_pid_maps, O_RDONLY);
    if (fd < 0)
        return false;
    

    char line[LINE_SIZE];
    size_t read_bytes;
    bool has_debug_libraries = false;

    while ((read_bytes = read(fd, line, LINE_SIZE)) > 0) {
        char *ptr = line;
        char *end = line + read_bytes;
        while ((ptr = StrStr(ptr, "libdebug")) != NULL) {
            if (ptr + 8 >= end || *(ptr + 8) == '\n') {
                has_debug_libraries = true;
                break;
            }
            ptr += 8;
        }
        if (has_debug_libraries) {
            break;
        }
    }

    close(fd);
    return has_debug_libraries;
}


bool check_for_errno() {
    errno = false;
    int value = errno;
    errno = value;
    if (errno != value) {
        return true;
    }
    return false;
}

bool check_for_breakpoint(){
    void *func = (void*)check_for_breakpoint;
    int pagesize = sysconf(_SC_PAGESIZE);
    if (mprotect((void*)((intptr_t)func & ~(pagesize - 1)), pagesize, PROT_READ | PROT_WRITE | PROT_EXEC) == -1) 
        return false;
    
    if (MemCompare(func, "\xCC", 1) == 0)
        return true;
    
    return false;
}

bool is_safe(){
    return ((ptrace(PTRACE_TRACEME, 0, 1, 0) == -1) || check_for_breakpoint() || check_for_errno() || check_for_debug_libraries());
}


bool is_safe_to_run(){
#ifdef DEBUG
    return true;
#endif
    return (!is_safe());
}
#else
bool is_safe_to_run(){ return true; }
#endif

