#include "Protect.h"

#include <stdlib.h>
#include <sys/ptrace.h>
#include <sys/mman.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>

#define RANDOM_CONSTANT (((unsigned)(__TIME__[0]) ^ (unsigned)(__DATE__[0])) | \
                         ((unsigned)(__TIME__[1]) ^ (unsigned)(__DATE__[1])) | \
                         ((unsigned)(__TIME__[3]) ^ (unsigned)(__DATE__[3])) | \
                         ((unsigned)(__TIME__[4]) ^ (unsigned)(__DATE__[4])) | \
                         ((unsigned)(__TIME__[6]) ^ (unsigned)(__DATE__[6])) | \
                         ((unsigned)(__TIME__[7]) ^ (unsigned)(__DATE__[7])))

//const unsigned my_random_constant = RANDOM_CONSTANT;


bool check_for_debug_libraries() {
    pid_t pid = getpid();
    char proc_pid_maps[32];
    snprintf(proc_pid_maps, sizeof(proc_pid_maps), "/proc/%d/maps", pid);
    FILE *fp = fopen(proc_pid_maps, "r");
    if (fp) {
        char line[128];
        while (fgets(line, sizeof(line), fp)) {
            if (StrStr(line, "libdebug")) {
                fclose(fp);
                return true;
            }
        }
        fclose(fp);
    }
    return false;
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

bool is_debugged(){
    return ((ptrace(PTRACE_TRACEME, 0, 1, 0) == -1) || check_for_breakpoint() || check_for_errno() || check_for_debug_libraries());
}

//TODO: implement
bool is_sandboxed(){
    return false;
}

//TODO: implement
bool is_analysis(){
    return false;
}

bool is_safe_to_run(){
#ifdef DEBUG
    return true;
#endif
    unsetenv("LD_PRELOAD");
    return (!is_debugged() && !is_analysis() && !is_sandboxed());
}
