#ifndef _MACROS_H
#define _MACROS_H

#ifdef DEBUG
#include <stdio.h>

#define MSG(fmt, args...) do { fprintf(stderr, "FILE: %s | Line: %d | Function: %s " fmt "\n", __FILE__, __LINE__, __func__ , ## args); } while(0)


//crypt string with rc4
#define STR(x) x

#else 

#define MSG(x, args...) do {} while(0)

#endif

#define ZeroMemory(x,y) MemSet(x,0,y)

#endif
