//

#define LINKAGE __declspec(dllexport)
#define HEAP
#define __thread __declspec(thread)

/*

#ifndef API
#define API
#define MALLOC(sz)    malloc(sz)
#define FREE(p)       (free(p), 0)
#define REALLOC(p,sz) realloc(p,sz)
#define STRDUP(x)     
#define HEAP
#ifdef _MSC_VER
#define __thread __declspec(thread)
#endif
#endif

#ifndef REALLOC
#include <string.h>
#include <stdlib.h>
#define REALLOC(p,n)   realloc(p,n) // defined in memory_malloc.c
#define MSIZE(p)       msize(p)     // defined in memory_malloc.c
#define CALLOC(c,n)    memset(MALLOC((c)*(n)), 0, (c)*(n))
#define MALLOC(n)      REALLOC(0, (n))
#define FREE(p)        REALLOC((p), 0)
#define STRDUP(s)      (char*)memcpy(MALLOC(strlen(s)+1), (s), strlen(s)+1)
#define WATCH(p,sz)    (p)
#define FORGET(p)      (p)
#endif

#ifdef _MSC_VER //#if MSC
#include <malloc.h>
#define ALLOCA(type,name,sz) type *name = (type*)_alloca(sz * sizeof(type))
#else
#define ALLOCA(type,name,sz) type name[sz]
#endif

*/
