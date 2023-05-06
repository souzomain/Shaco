#include "Default.h"
#include "shaco_stdlib.h"
#include <time.h>

#define MT19937_N 624
#define MT19937_M 397

typedef struct {
    uint32_t state[MT19937_N];
    int index;
} mt19937_state_t;

void mt19937_init(mt19937_state_t *state, uint32_t seed) {
    state->state[0] = seed;
    for (int i = 1; i < MT19937_N; i++) {
        state->state[i] = 1812433253UL * (state->state[i-1] ^ (state->state[i-1] >> 30)) + i;
    }
    state->index = MT19937_N;
}

uint32_t mt19937_next(mt19937_state_t *state) {
    if (state->index >= MT19937_N) {
        for (int i = 0; i < MT19937_N; i++) {
            uint32_t y = (state->state[i] & 0x80000000UL) + (state->state[(i+1) % MT19937_N] & 0x7fffffffUL);
            state->state[i] = state->state[(i+MT19937_M) % MT19937_N] ^ (y >> 1);
            if (y & 1) {
                state->state[i] ^= 0x9908b0dfUL;
            }
        }
        state->index = 0;
    }
    uint32_t y = state->state[state->index++];
    y ^= (y >> 11);
    y ^= (y << 7) & 0x9d2c5680UL;
    y ^= (y << 15) & 0xefc60000UL;
    y ^= (y >> 18);
    return y;
}

int generate_random_int(int min, int max){
    static mt19937_state_t state;
    static int initialized = 0;

    if (!initialized) {
        mt19937_init(&state, time(NULL));
        initialized = 1;
    }

    return (int)(mt19937_next(&state) % (max - min + 1)) + min;
}


char* generate_random_str(int length) {
    char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    int charset_size = StringLength(charset);

    char* str = shaco_malloc(length + 1);
    for (int i = 0; i < length; i++) {
        int random_index = generate_random_int(0, charset_size - 1);
        str[i] = charset[random_index];
    }
    str[length] = '\0';

    return str;
}

int s_atoi(char *str, bool *ok){
    bool success = false;
    int num = 0, signal = 1;
    char *tmp = str;
    if(*tmp == '-'){
        signal = -1;
        ++tmp;
    }

    while(*tmp != '\0'){
        if(*tmp < '0' || *tmp > '9')
            goto EXIT;
        num = num * 10 + ( *tmp - '0');
        ++tmp;
    }
    if(!str) goto EXIT;
EXIT:
    if(ok != NULL) *ok = success;
    return num * signal;
}
