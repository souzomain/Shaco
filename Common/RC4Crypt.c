#include "RC4Crypt.h"
#include "shaco_stdlib.h"
void rc4_init(rc4_t *state, uint8_t *key, int32_t len){
    if( !key || len <=0 ) return;
    if(!state) state = (rc4_t *)shaco_malloc(sizeof(rc4_t));
    int32_t i, j, k, *m, a;

    state->x = 0;
    state->y = 0;
    m = state->m;

    for( i = 0; i < 256; i++ ) m[i] = i;

    j = k = 0;

    for( i = 0; i < 256; i++ )
    {
        a = m[i];
        j = (uint8_t) ( j + a + key[k] );
        m[i] = m[j]; m[j] = a;
        if( ++k >= len ) k = 0;
    }
}

bool rc4_crypt(rc4_t *state, uint8_t *data, int32_t len){
    if(!state || !data || len <= 0) return false;
    int i, x, y, *m, a, b;

    x = state->x;
    y = state->y;
    m = state->m;

    for( i = 0; i < len ; i++ )
     {
        x = (uint8_t) ( x + 1 ); a = m[x];
        y = (uint8_t) ( y + a );
        m[x] = b = m[y];
        m[y] = a;
        data[i] ^= m[(uint8_t) ( a + b )];
    }

    state->x = x;
    state->y = y;
    return true;
}
