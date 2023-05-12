#ifndef _DEFAULT_H
#define _DEFAULT_H

#include "Macros.h"
#include "Defines.h"
#include <signal.h>
#include <sys/sysinfo.h>
#include <sys/utsname.h>

void s__exit(int v);

char*   generate_random_str(int max);
int     generate_random_int(int min, int max);

int     s_atoi(char *str, bool *ok);
int     s_chdir(const char *path);
int     s_nanosleep(const struct timespec *requested_time, struct timespec *remaining);
int     s_prctl(int option, char name[]);

mode_t  s_umask(__mode_t mask);

__pid_t s_fork();
__pid_t s_setsid();
__pid_t s_getpid();
__pid_t s_getppid();

__uid_t s_getuid();

__sighandler_t s_signal(int sig, __sighandler_t handler);

time_t s_time(time_t *timer);

int s_sysinfo(struct sysinfo *info);
int s_uname(struct utsname *name);

#endif
