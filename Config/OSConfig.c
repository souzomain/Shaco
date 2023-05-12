#include "OSConfig.h"
#include "../Common/shaco_stdlib.h"
#include "../Common/Network.h"

#include <netinet/in.h>
#include <unistd.h>
#include <sys/sysinfo.h>
#include <string.h>
#include <ifaddrs.h>
#include <sys/utsname.h>


bool get_internal_ip(char *mem){
    if(!mem) return false;
    struct ifaddrs *ifaddrs = NULL, *ifa = NULL;
    if(getifaddrs(&ifaddrs) == -1) { return false;}
    ifa = ifaddrs;
    while(ifa){
        if(ifa->ifa_addr && (ifa->ifa_name[0] == 'l' && ifa->ifa_name[1] == 'o' && ifa->ifa_name[3] == '\0') != 1 && ifa->ifa_addr->sa_family == AF_INET ){
            struct sockaddr_in *pAddr = (struct sockaddr_in *)ifa->ifa_addr;
            uint32_t ip = s_ntohl(pAddr->sin_addr.s_addr);
            s_inet_ntop(AF_INET, &(pAddr->sin_addr), mem, INET_ADDRSTRLEN);
            freeifaddrs(ifaddrs);
            return true;
        }
        ifa = ifa->ifa_next;
    }

    freeifaddrs(ifaddrs);
    return false;
}

POSCONFIG get_os_config(){
    POSCONFIG cfg = (POSCONFIG)shaco_calloc(sizeof(OSCONFIG),1);
    if(!cfg) return NULL;

    struct sysinfo mem_info;
    struct utsname info;

    if(s_sysinfo(&mem_info) != 0 ) goto EXIT_FAIL;
    if(getlogin_r(cfg->user, LOGIN_NAME_MAX + 1) != 0) goto EXIT_FAIL;
    if(gethostname(cfg->hostname, sizeof(cfg->hostname)) != 0) goto EXIT_FAIL;
    if(!get_internal_ip(cfg->internal_ip)) {
        MSG("Can't get internal ip config");
        StringCopy(cfg->internal_ip, "127.0.0.1");
    }
    if(s_uname(&info) < 0) {
        StringCopy(cfg->arch, "(none)");
        StringCopy(cfg->version, "(none)");
    }else {
        StringCopy(cfg->version, info.release);
        StringCopy(cfg->arch, info.machine);
    }

    getdomainname(cfg->domain,254);
    cfg->pid = s_getpid();
    cfg->elevated = (s_getuid() == 0 ? false : true);
    cfg->ram_size = mem_info.totalram;
    cfg->processors = get_nprocs();
    return cfg;

EXIT_FAIL:
    ZeroMemory(cfg, sizeof(OSCONFIG));
    shaco_free(cfg);
    return NULL;
}

void osconfig_free(POSCONFIG cfg){
    if(!cfg) return;
    ZeroMemory(cfg, sizeof(OSCONFIG));
    shaco_free(cfg);
    cfg = NULL;
}

