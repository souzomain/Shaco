#ifndef _S_STDLIB_H
#define _S_STDLIB_H
#include <stddef.h>
#include <bits/types.h>

void *  s_mmap(void *addr, size_t len, int prot, int flags, int fd, __off_t offset);
int     s_munmap(void *addr, size_t len);
int     s_mprotect(void *addr, size_t len, int prot);

void *shaco_malloc(size_t len);
void *shaco_calloc(size_t nmemb, size_t size);
void *shaco_realloc(void * ptr, size_t size);
void  shaco_free(void *);

int     StringCompare(const char *str1, const char *str2);
char*   StringCopy(char *str1, char *str2);
size_t  StringLength(char *str);
char*   StrStr(const char* haystack, const char* needle);
char*   StrStrN(const char* haystack, const char* needle, size_t *size);
char*   StringCat(char* dest, const char* src);

void    CopyMemory(void *dest, const void *src, size_t n);
void    MemSet(void *x, int val, size_t y);
int     MemCompare(const void* s1, const void* s2, size_t n);
void*   MemCat(void *data1, size_t len1, void *data2, size_t len2);

int s_snprintf(char *str, size_t size, const char *format, ...);
int s_sscanf(const char *str, const char *format, ...);

char*   s_strdup(char *str);

#endif
