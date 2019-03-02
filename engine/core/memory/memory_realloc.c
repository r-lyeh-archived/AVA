// realloc replacement that dies when out of memory.

#ifndef REALLOC_H
#define REALLOC_H
#include <stdlib.h>
#include <string.h>

#include "../detect/detect_memory.c" // macros

API void *reallocx( void *ptr, int size );
API int   msizex( void *ptr );

// forward declarations (in case memory leaks are watched)
API void* watch( void *ptr, int sz );
API void* forget( void *ptr );

#endif


#ifdef REALLOC_C
#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#ifndef MSIZE_
#include <stdint.h>
#if defined(__GLIBC__)
#  include <malloc.h>
#  define MSIZE_ malloc_usable_size
#elif defined(__APPLE__) || defined(__FreeBSD__)
#  include <malloc/malloc.h>
#  define MSIZE_  malloc_size
#elif defined(__ANDROID_API__)
#  include <malloc.h>
/*extern "C"*/ size_t dlmalloc_usable_size(void*);
#  define MSIZE_ dlmalloc_usable_size
#elif defined(_WIN32)
#  include <malloc.h>
#  define MSIZE_ _msize
#else
#  error Unsupported malloc_usable_size()
#endif
#endif

int msizex( void *p) {
    if( p ) return MSIZE_(p);
    return 0;
}

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

void *reallocx(void *ptr, int size) {
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

#endif
