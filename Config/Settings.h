#ifndef _SETTINGS_H
#define _SETTINGS_H

#include "../Common/Default.h"

enum{
    SETTINGS_VERSION_LENGTH = 10
};

typedef struct{
    char domain[MAX_DOMAIN_LENGTH];
    char version[SETTINGS_VERSION_LENGTH];
    char ip[INET6_ADDRSTRLEN];
    char useragent[200];
    char endpoint[200];
    int agent_id;
    int port;
    int timeout;
    int jitter;
    bool quit;
    bool ssl;
    bool close;
}SETTINGS, *PSETTINGS;

PSETTINGS get_settings();

//TODO: implement get remote settings
#ifdef SETTINGS_GET_REMOTE
PSETTINGS get_remote_settings();
#endif

#endif
