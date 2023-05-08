#include "Settings.h"

#include "../Common/shaco_stdlib.h"

#ifdef DEBUG
#define IP "127.0.0.1"
#define ENDPOINT "/"
#define DOMAIN "google.com"
#define USERAGENT "Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/96.0.4664.110 Safari/537.36"
#define TIMEOUT 5
#define PORT 8080
#define MAXTIMEOUT 0
#else
#define IP "%IP%"
#define ENDPOINT "%ENDPOINT%"
#define DOMAIN "%DOMAIN%"
#define USERAGENT "%USERAGENT%"
//#define VERSION "%VERSION%"
#define TIMEOUT %TIMEOUT%
#define MAXTIMEOUT %MAXTIMEOUT%
#define PORT %PORT%
#endif

static PSETTINGS sett = NULL;
PSETTINGS get_settings(){
    if(sett != NULL) return sett;

    int port = PORT;
    int timeout = TIMEOUT;
    int jitter = MAXTIMEOUT;
    uint8_t domain[] = DOMAIN;
    uint8_t ip[] = IP;
    uint8_t endpoint[] = ENDPOINT;
    uint8_t useragent[] = USERAGENT;
//    uint8_t version[] = VERSION;


    PSETTINGS set = (PSETTINGS)shaco_calloc(sizeof(SETTINGS),1);
    if(!set) return NULL;

    StringCopy(set->domain, (char *)domain);
    StringCopy(set->ip, (char *)ip);
    StringCopy(set->endpoint, (char *)endpoint);
    StringCopy(set->useragent, (char *)useragent);
    
    set->port = port;
    set->ssl = false;
    set->timeout = timeout;
    set->max_timeout = jitter;
    sett = set;
    return set;
}

#ifdef SETTINGS_GET_REMOTE
PSETTINGS get_remote_settings(){
    PSETTINGS remote_info = get_settings();

    //TODO: get info from remote and return new settings
    return NULL;
}
#endif
