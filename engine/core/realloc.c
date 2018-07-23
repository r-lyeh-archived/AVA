// realloc replacement that dies when out of memory.

#ifndef REALLOC_H
#define REALLOC_H
#include <stdlib.h>
#include <string.h>

API void *REALLOC( void *ptr, int size );
API int   MSIZE( void *ptr );

#define CALLOC(c,n)    memset(MALLOC((c)*(n)), 0, (c)*(n))
#define MALLOC(n)      REALLOC(0, (n))
#define FREE(p)        REALLOC((p), 0)
#define STRDUP(s)      (char*)memcpy(MALLOC(strlen(s)+1), (s), strlen(s)+1)
#define WATCH(p,sz)    (p)
#define FORGET(p)      (p)

// forward declarations (in case memory leaks are watched)
API void* watch( void *ptr, int sz );
API void* forget( void *ptr );

#endif


#ifdef REALLOC_C
#pragma once
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

static char *oom = 0;
#ifdef AUTORUN
AUTORUN {
    oom = malloc(16 * 1024 * 1024);
}
#endif

static
void panic_handler(const char *msg) {
    puts( msg );  // LOGERROR(PANIC, msg);
    exit( ENOMEM );
}

void *REALLOC(void *ptr, int size) {
    ptr = (realloc)(ptr, size);
    if( size > 0 ) if( !ptr ) {
        free(oom), oom = 0;
        panic_handler("!error: out-of-memory");
    }
#ifdef REALLOC_POISON
    if( size > 0 ) if( ptr ) {
        memset(ptr, 0xCD, size);
    }
#endif
    return ptr;
}

int MSIZE( void *p) {
    if( p ) return _msize(p);
    return 0;
}

#endif
