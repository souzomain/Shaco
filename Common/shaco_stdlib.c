#define _GNU_SOURCE

#include "shaco_stdlib.h"
#include "shaco_syscall.h"
#include "Default.h"

#include <sys/mman.h>
#include <stdio.h>
#include <stdarg.h>

#define HEADER_SIZE sizeof(size_t)

#define PAGE_SIZE sysconf(_SC_PAGESIZE)

void * s_mmap(void *addr, size_t len, int prot, int flags, int fd, __off_t offset){
    void *ret = NULL;
    addr = (void*)(((unsigned long)addr + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1));
    len = (len + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);
    ret = NULL;
#if defined(__x86_64__)
    asm volatile(
        "mov $9, %%rax\n"
        "mov %1, %%rdi\n"
        "mov %2, %%rsi\n"
        "mov %3, %%rdx\n"
        "mov %4, %%r10\n"
        "mov %5, %%r8\n"
        "mov %6, %%r9\n"
        "syscall\n"
        "mov %%rax, %0\n"
        : "=r" (ret)
        : "g" (addr), "g" (len), "g" (prot), "g" (flags), "g" (fd), "g" (offset)
        : "%rax", "%rdi", "%rsi", "%rdx", "%r10", "%r8", "%r9", "memory", "cc"
    );
#elif defined(__aarch64__)
    __asm__ volatile(
        "mov x8, #192\n"
        "mov x0, %1\n"
        "mov x1, %2\n"
        "mov x2, %3\n"
        "mov x3, %4\n"
        "mov x4, %5\n"
        "mov x5, %6\n"
        "svc 0\n"
        "mov %0, x0\n"
        : "=r"(ret)
        : "r"(addr), "r"(len), "r"(prot), "r"(flags), "r"(fd), "r"(offset)
        : "x0", "x1", "x2", "x3", "x4", "x5", "x8", "memory", "cc"
    );
#else
    ret = mmap(addr,len, prot, flags, fd, offset);
#endif
    return ret;
}

int s_munmap(void *addr, size_t len){
    return shaco_syscall(SYS_munmap, addr, len);
}

int s_mprotect(void *addr, size_t len, int prot){
    if(!addr) return -1;
    return shaco_syscall(SYS_mprotect, addr, len, prot);
}

void *shaco_malloc(size_t size) {
    void *ptr = s_mmap(NULL, size + HEADER_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (ptr == MAP_FAILED){
        MSG("can't allocate memory");
        return NULL;
    }

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
    s_munmap((void *)((char *)ptr - HEADER_SIZE), size + HEADER_SIZE);
}


int StringCompare( const char *String1, const char *String2 )
{
    for (; *String1 == *String2; String1++, String2++)
    {
        if (*String1 == '\0')
            return 0;
    }

    return ((*(const char *)String1 < *(const char *)String2) ? -1 : +1);

}

char *StringCopy(char *String1, char *String2)
{
    char *p = String1;

    while ((*p++ = *String2++) != 0);

    return String1;
}

size_t StringLength(char *String)
{
    char *String2;

    if ( String == NULL )
        return 0;

    for (String2 = String; *String2; ++String2);

    return (String2 - String);
}



void MemSet(void *Destination, int Val, size_t Size)
{
    unsigned long * Dest = ( unsigned long * )Destination;
    size_t Count = Size / sizeof( unsigned long );

    while ( Count > 0 )
    {
        *Dest = Val;
        Dest++;
        Count--;
    }
}

int MemCompare( const void* s1, const void* s2, size_t n)
{
    const unsigned char* p1 = s1;
    const unsigned char* p2 = s2;

    for (size_t i = 0; i < n; i++) 
        if (p1[i] != p2[i]) 
            return (p1[i] < p2[i]) ? -1 : 1;
    
    return 0;}

void CopyMemory(void *dest, const void *src, size_t n) {
    char *cdest = (char *)dest;
    const char *csrc = (const char *)src;

    for (size_t i = 0; i < n; i++) {
        cdest[i] = csrc[i];
    }
}

char* StrStr(const char* haystack, const char* needle) {
    if (haystack == NULL || needle == NULL)
        return NULL;

    if (*needle == '\0')
        return (char*) haystack;

    const char* current = haystack;
    while (*current != '\0') {
        const char* h = current;
        const char* n = needle;

        while (*h == *n && *h != '\0') {
            h++;
            n++;
        }

        if (*n == '\0') 
            return (char*) current;

        current++;
    }

    return NULL; 
}

char* StrStrN(const char* haystack, const char* needle, size_t *size) {
    if (haystack == NULL || needle == NULL)
        return NULL;

    if (*needle == '\0'){
        if(size != NULL) *size = 0;
        return (char*) haystack;
    }
    const char* current = haystack;
    while (*current != '\0') {
        const char* h = current;
        const char* n = needle;

        while (*h == *n && *h != '\0') {
            h++;
            n++;
        }

        if (*n == '\0') {
            if(size != NULL) *size = current - haystack;
            return (char*) current;
        }
        current++;
    }

    return NULL; 
}

char* StringCat(char* dest, const char* src) {
    char* ptr = dest + StringLength(dest);
    while(*src != '\0') {
        *ptr++ = *src++;
    }
    *ptr = '\0';
    return dest;
}

void *MemCat(void *data1, size_t len1, void *data2, size_t len2) {
    if(!data1 || !data2 || len1 <= 0 || len2 <= 0) return NULL;

    void *result = shaco_malloc(len1 + len2);
    if (!result) {
        return NULL;
    }
    CopyMemory(result, data1, len1);
    CopyMemory(result + len1, data2, len2);
    return result;
}


int s_snprintf(char *str, size_t size, const char *format, ...){
    va_list args;
    va_start(args, format);

    int result = vsnprintf(str, size, format, args);

    va_end(args);

    return result;
}

int s_sscanf(const char *str, const char *format, ...){
    va_list args;
    va_start(args, format);

    int result = vsscanf(str, format, args);

    va_end(args);

    return result;
}

char *s_strdup(char *str){
    size_t len = StringLength(str);
    char *dup = shaco_malloc(len + 1);
    if(!dup) return NULL;
    StringCopy(dup, str);
    return dup;
}
