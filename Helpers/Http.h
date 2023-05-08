#ifndef _HTTP_H
#define _HTTP_H

#include "../Common/Default.h"

typedef struct http_response{
    int status;
    size_t size;
    uint8_t *response;
    uint8_t *buffer;
    size_t buffer_size;
}HTTP_RESPONSE, *PHTTP_RESPONSE;

typedef struct http_request{
    char host[256];
    int port;
    bool ssl;
    void *data;
    size_t data_size;
}HTTP_REQUEST, *PHTTP_REQUEST;

PHTTP_RESPONSE shaco_http_post(void *postfield, uint64_t postfield_size);
void http_free(PHTTP_RESPONSE resp);
#endif
