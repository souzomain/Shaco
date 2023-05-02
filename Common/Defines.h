#ifndef _DEFINES_H
#define _DEFINES_H
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

typedef uint8_t u8;
typedef uint32_t u32;
typedef uint64_t u64;

#define HOST_NAME_MAX		64
#define LOGIN_NAME_MAX		256
#define IF_NAMESIZE	16
#define IFNAMSIZ	IF_NAMESIZE
#define INET_ADDRSTRLEN 16
#define INET6_ADDRSTRLEN 46
#define MAX_DOMAIN_LENGTH 256

#include <sys/types.h>

#endif
