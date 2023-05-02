#ifndef _MINISTD_H
#define _MINISTD_H
#include "Defines.h"

u32     StringCompare(const char *str1, const char *str2);
char*   StringCopy(char *str1, char *str2);
size_t  StringLength(char *str);

void    MemSet(void *x, u32 val, size_t y);
u32     MemCompare(void *x, void *y, u32 len);
void    CopyMemory(void *dest, const void *src, size_t n);
char*   StrStr(const char* haystack, const char* needle);
char* StringCat(char* dest, const char* src);
#endif
