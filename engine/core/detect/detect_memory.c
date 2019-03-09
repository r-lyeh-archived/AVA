#pragma once
#include <string.h>

#ifndef REALLOC
#define REALLOC(p,n)   reallocx(p,n) // defined in memory_malloc.c
#define MSIZE(p)       msizex(p)     // defined in memory_malloc.c
#define CALLOC(c,n)    memset(MALLOC((c)*(n)), 0, (c)*(n))
#define MALLOC(n)      REALLOC(0, (n))
#define FREE(p)        REALLOC((p), 0)
#define STRDUP(s)      (char*)memcpy(MALLOC(strlen(s)+1), (s), strlen(s)+1)
#define WATCH(p,sz)    (p)
#define FORGET(p)      (p)
#endif

#ifdef _MSC_VER //#if MSC
#include <malloc.h>
#define ALLOCA(type,name,sz) type *name = (type*)memset(_alloca(sz * sizeof(type)), 0, (sz * sizeof(type))) // memset needed?
#else
#define ALLOCA(type,name,sz) type name[sz] = {0}
#endif

