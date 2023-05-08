#include "Network.h"
#include "Default.h"
#include "shaco_stdlib.h"
#include "shaco_syscall.h"
#include <sys/select.h>

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
    s_snprintf(dst, size, "%d.%d.%d.%d", bytes[0], bytes[1], bytes[2], bytes[3]);
    return dst;
}

int s_open(const char *file, int oflag, mode_t mod){
    return shaco_syscall(SYS_open,file,oflag, mod);
}

int s_close(int fd){
    return shaco_syscall(SYS_close,fd);
}

ssize_t s_write(int fd, const void *buf, size_t n){
    if(fd < 0 ) return -1;
    return shaco_syscall(SYS_write, fd, buf, n);
}

ssize_t s_read(int fd, void *buf, size_t nbytes){
    if(fd < 0) return -1;
    return shaco_syscall(SYS_read, fd, buf, nbytes);
}

int s_connect(int fd, const struct sockaddr *addr, socklen_t len){
    if(fd < 0) return -1;
    int ret = shaco_syscall(SYS_connect, fd, addr, len);
    if(ret < 0 || ret >= FD_SETSIZE) return -1;
    return ret;
}

int s_socket(int domain, int type, int protocol){
    return shaco_syscall(SYS_socket, domain, type, protocol);
}
