#include "Settings.h"

#include "../Common/shaco_stdlib.h"

#define IP "127.0.0.1"
#define ENDPOINT ""
#define DOMAIN "google.com"
#define USERAGENT "Mozilla/5.0 (X11; Linux x86_64; rv:109.0) Gecko/20100101 Firefox/112.0"
#define VERSION "0.1"
#define TIMEOUT 10
#define PORT 8080
#define USESSL false

static PSETTINGS settings = NULL;

PSETTINGS get_settings(){
    
    if(settings != NULL) return settings;

    PSETTINGS set = (PSETTINGS)shaco_calloc(sizeof(SETTINGS),1);
    if(!set) return NULL;
    StringCopy(set->domain, DOMAIN);
    StringCopy(set->ip, IP);
    StringCopy(set->endpoint, ENDPOINT);
    StringCopy(set->useragent, USERAGENT);
    StringCopy(set->version, VERSION); //TODO: get atualization
    
    set->port = PORT;
    set->quit = false;
    set->ssl = USESSL;
    set->close = true;
    set->timeout = TIMEOUT;
    set->jitter = 0; //TODO: implement this
    settings = set;
    return set;
}


#ifdef SETTINGS_GET_REMOTE
PSETTINGS get_remote_settings(){
    PSETTINGS remote_info = get_settings();

    //TODO: get info from remote and return new settings
    return NULL;
}
#endif
