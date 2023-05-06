#include "Network.h"
#include "Default.h"
#include "shaco_stdlib.h"

#include <netdb.h>
#include <stdio.h>


uint32_t s_ntohl(uint32_t netlong) {
    uint32_t hostlong = 0;
    int i;

    for (i = 0; i < 4; i++) {
        hostlong <<= 8;
        hostlong |= (netlong >> (24 - i*8)) & 0xff;
    }

    return hostlong;
}



uint32_t s_htonl(uint32_t hostlong) {
    uint32_t netlong = 0;
    int i;

    for (i = 0; i < 4; i++) {
        netlong <<= 8;
        netlong |= (hostlong & 0xff);
        hostlong >>= 8;
    }

    return netlong;
}

uint16_t s_htons(uint16_t hostshort){
    return ((hostshort & 0xFF) << 8) | ((hostshort >> 8) & 0xFF);
}


unsigned long s_inet_addr(const char *cp) {
    unsigned long addr = 0;
    int shift = 24;
    int octet = 0;
    int count = 0;

    while (*cp) {
        if (*cp >= '0' && *cp <= '9') {
            octet = octet * 10 + (*cp - '0');
            if (octet > 255) {
                return INADDR_NONE;
            }
        } else if (*cp == '.') {
            if (count > 3) {
                return INADDR_NONE;
            }
            addr |= octet << shift;
            octet = 0;
            shift -= 8;
            count++;
        } else {
            return INADDR_NONE;
        }
        cp++;
    }

    if (count != 3) {
        return INADDR_NONE;
    }

    addr |= octet << shift;

    return s_htonl(addr);
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

