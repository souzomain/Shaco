#ifndef _RC4CRYPT_H
#define _RC4CRYPT_H

#include <stdint.h>
#include <stdbool.h>
typedef struct {
    int32_t x, y, m[256];
}rc4_t;

void rc4_init(rc4_t *state, uint8_t *key, int32_t len);
bool rc4_crypt(rc4_t *state, uint8_t *data, int32_t len);

#endif
