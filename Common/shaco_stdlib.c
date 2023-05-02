#include "shaco_stdlib.h"

#include "Default.h"
#include <stdlib.h>

void *shaco_malloc(unsigned long len){
    return malloc(len);
}

void *shaco_calloc(size_t nmemb, size_t size){
    size_t total_size = nmemb * size;
    void *ptr = shaco_malloc(total_size);
    if (ptr != NULL) 
        ZeroMemory(ptr, total_size);
    
    return ptr;
}

void *shaco_realloc(void * ptr, unsigned long size){
    if(size <= 0) {free(ptr); return NULL;}
    return realloc(ptr, size);
}

void shaco_free(void *ptr){
    if(!ptr) return;
    free(ptr);
}

