#ifndef _OSCONFIG_H
#define _OSCONFIG_H

#include "../Common/Default.h"

typedef struct {
    char hostname[HOST_NAME_MAX + 1];
    char user[LOGIN_NAME_MAX + 1];
    char internal_ip[INET6_ADDRSTRLEN];
    char domain[255];
    bool elevated;
    pid_t pid;
    unsigned long ram_size;
    int processors;
}OSCONFIG, *POSCONFIG;


POSCONFIG get_os_config();
#endif
