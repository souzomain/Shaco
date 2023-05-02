#ifndef _S_STDLIB_H
#define _S_STDLIB_H
void *shaco_malloc(unsigned long len);
void *shaco_calloc(unsigned long nmemb, unsigned long size);
void *shaco_realloc(void * ptr, unsigned long size);
void shaco_free(void *);
#endif
