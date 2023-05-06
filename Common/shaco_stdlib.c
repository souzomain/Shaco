#define _GNU_SOURCE

#include "shaco_stdlib.h"
#include "Default.h"
#include <sys/mman.h>

#define HEADER_SIZE sizeof(size_t)
void *shaco_malloc(size_t size) {
    void *ptr = mmap(NULL, size + HEADER_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (ptr == MAP_FAILED) 
        return NULL; 

    *((size_t *)ptr) = size;

    return (void *)((char *)ptr + HEADER_SIZE); 
}


void *shaco_calloc(size_t nmemb, size_t size){
    size_t total_size = nmemb * size;
    void *ptr = shaco_malloc(total_size);
    if (ptr != NULL) 
        ZeroMemory(ptr, total_size);
    
    return ptr;
}

void *shaco_realloc(void * ptr, size_t size){

    if (ptr == NULL) 
        return shaco_malloc(size);

    size_t old_size = *((size_t *)((char *)ptr - HEADER_SIZE));

    if (size <= old_size) 
        return ptr;
    
    void *new_ptr = shaco_malloc(size);
    if (new_ptr == NULL) 
        return NULL;
    
    CopyMemory(new_ptr, ptr, old_size);
    shaco_free(ptr);
    return new_ptr;

}

void shaco_free(void *ptr) {
    if (ptr == NULL) return;
    size_t size = *((size_t *)((char *)ptr - HEADER_SIZE));
    ZeroMemory(ptr, size);
    munmap((void *)((char *)ptr - HEADER_SIZE), size + HEADER_SIZE);
}
