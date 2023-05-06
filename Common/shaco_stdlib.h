#ifndef _S_STDLIB_H
#define _S_STDLIB_H
#include <stddef.h>
void *shaco_malloc(size_t len);
void *shaco_calloc(size_t nmemb, size_t size);
void *shaco_realloc(void * ptr, size_t size);
void shaco_free(void *);
#endif
