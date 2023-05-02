#include "ministd.h"

u32 StringCompare( const char *String1, const char *String2 )
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



void MemSet(void *Destination, u32 Val, size_t Size)
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

u32 MemCompare( void *s1, void *s2, u32 len)
{
    u8 * p = s1;
    u8 *q = s2;
    u32 charCompareStatus = 0;

    if (s1 == s2)
        return charCompareStatus;

    while (len > 0)
    {
        if (*p != *q)
        {
            charCompareStatus = (*p >*q)?1:-1;
            break;
        }
        len--;
        p++;
        q++;
    }
    return charCompareStatus;
}

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

char* StringCat(char* dest, const char* src) {
    char* ptr = dest + StringLength(dest);
    while(*src != '\0') {
        *ptr++ = *src++;
    }
    *ptr = '\0';
    return dest;
}
