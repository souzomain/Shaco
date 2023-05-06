#ifndef _NETWORK_H
#define _NETWORK_H
#include "Default.h"
#include <arpa/inet.h>


uint32_t s_ntohl(uint32_t value);
uint16_t s_htons(uint16_t hostshort);
char* s_inet_ntoa(struct in_addr in);
//bool get_ip_host(char hostname[], char *dest);
const char *s_inet_ntop(int af, const void *src, char *dst, socklen_t size);
unsigned long s_inet_addr(const char* ip_str);

#endif
