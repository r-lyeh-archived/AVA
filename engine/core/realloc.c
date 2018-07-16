#ifndef REALLOC_H
#define REALLOC_H
#include <string.h>

void *REALLOC( void *ptr, int size );
int MSIZE( void *ptr );

#define CALLOC(c,n)    memset(MALLOC((c)*(n)), 0, (c)*(n))
#define MALLOC(n)      REALLOC(0, (n))
#define FREE(p)        REALLOC((p), 0)
#define STRDUP(s)      (char*)memcpy(MALLOC(strlen(s)+1), (s), strlen(s)+1)

#endif


#ifdef REALLOC_C
#pragma once
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h> 

static
void panic_handler(const char *msg) {
    puts( msg );  // LOGERROR(PANIC, msg);
    exit( -1 ); // ENOMEM
}

// realloc replacement that dies when out of memory

void *REALLOC(void *ptr, int size) {
    ptr = (realloc)(ptr, size);
    if( size > 0 ) if( !ptr ) {
        //free(oom);
        //outofmem_exception();
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
