#ifndef _HTTP_H
#define _HTTP_H

#include "../Common/Default.h"

typedef struct http_response{
    int status;
    size_t size;
    char *response;
}HTTP_RESPONSE, *PHTTP_RESPONSE;

typedef struct http_request{
    char host[256];
    int port;
    bool ssl;
    void *data;
    size_t data_size;
}HTTP_REQUEST, *PHTTP_REQUEST;

PHTTP_RESPONSE http_post(char ip[],char host[], char endpoint[], char useragent[], int port, bool ssl, void *postfield, size_t postfield_size);
void http_free(PHTTP_RESPONSE resp);
#endif
