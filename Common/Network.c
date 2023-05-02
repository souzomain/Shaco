#include "Network.h"
#include <netdb.h>
#include <stdio.h>

uint32_t s_ntohl(uint32_t netlong)
{
    uint8_t *p = (uint8_t *) &netlong;
    return ((uint32_t)p[0] << 24) | ((uint32_t)p[1] << 16) | ((uint32_t)p[2] << 8) | ((uint32_t)p[3] << 0);
}


uint32_t s_htonl(uint32_t hostlong)
{
    uint8_t *p = (uint8_t *) &hostlong;
    return ((uint32_t)p[0] << 0) | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}


uint16_t s_htons(uint16_t hostshort){
    return ((hostshort & 0xFF) << 8) | ((hostshort >> 8) & 0xFF);
}

uint32_t s_inet_addr(const char* ip_str){
    uint32_t ip = 0;
    int i;
    int octet;
    for (i = 0; ip_str[i] != '\0'; i++) {
        if (ip_str[i] == '.') {
            ip = (ip << 8) | octet;
            octet = 0;
        } else if (ip_str[i] >= '0' && ip_str[i] <= '9') 
            octet = (octet * 10) + (ip_str[i] - '0');
        else 
            return INADDR_NONE;
        
    }
    ip = (ip << 8) | octet;
    return s_htonl(ip);
}

const char *s_inet_ntop(int af, const void *src, char *dst, socklen_t size)
{
    if (af != AF_INET || !src || !dst || size < INET_ADDRSTRLEN) {
        return NULL;
    }
    const unsigned char *bytes = (const unsigned char *)src;
    snprintf(dst, size, "%d.%d.%d.%d", bytes[0], bytes[1], bytes[2], bytes[3]);
    return dst;
}


/*
bool get_ip_host(char hostname[], char *dest) {
    struct hostent *host = gethostbyname(hostname);
    if(!host)
        return false;
    struct in_addr** addr_list = (struct in_addr**) host->h_addr_list;
    if (addr_list[0] == NULL) 
        return false;
    
    StringCopy(dest, s_inet_ntoa(*addr_list[0]));
    dest[INET_ADDRSTRLEN-1] = '\0';
    return true;
}
*/
