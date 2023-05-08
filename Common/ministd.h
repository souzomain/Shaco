#ifndef _MINISTD_H
#define _MINISTD_H
#include "Defines.h"

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
#endif
