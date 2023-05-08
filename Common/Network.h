#ifndef _NETWORK_H
#define _NETWORK_H
#include "Default.h"
#include <arpa/inet.h>

uint32_t        s_ntohl(uint32_t value);
uint16_t        s_htons(uint16_t hostshort);
char*           s_inet_ntoa(struct in_addr in);
const char*     s_inet_ntop(int af, const void *src, char *dst, socklen_t size);
unsigned long   s_inet_addr(const char* ip_str);

int     s_open(const char *file, int oflag, mode_t mod);
int     s_close(int fd);
int     s_connect(int fd, const struct sockaddr *addr, socklen_t len);
int     s_socket(int domain, int type, int protocol);
ssize_t s_write(int fd, const void *buf, size_t n);
ssize_t s_read(int fd, void *buf, size_t nbytes);
#endif
