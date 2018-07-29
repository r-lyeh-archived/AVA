#ifndef MALLOCA_H
#define MALLOCA_H
#include <stdarg.h>

/*STACK*/ char *malloca(int bytes);
/*STACK*/ char *mallocaf(const char *fmt, ...);
/*STACK*/ char *mallocav(const char *fmt, va_list va);

#endif

#ifdef MALLOCA_C
#pragma once
#include "../detect/detect_builtin.c"
#include "../core/core_realloc.c"
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

static builtin(thread) uint8_t *stack_mem = 0;
static builtin(thread) uint64_t stack_size = 0, stack_max = 0;
/*stack*/ char *malloca(int bytes) {
    if( bytes < 0 ) {
        if( stack_size > stack_max ) stack_max = stack_size;
        return (stack_size = 0), NULL;
    }
    if( !stack_mem ) stack_mem = REALLOC( stack_mem, MSIZE(stack_mem) + 4 * 1024 * 1024 );
    return &stack_mem[ (stack_size += bytes) - bytes ];
    /*
    inc_stats('pile', bytes);
    return &(vamem = (uint8_t*)realloc( vamem, vasz += bytes))[ vasz - bytes ];
    */
}
/*stack*/ char *mallocaf( const char *fmt, ... ) {
    va_list vl;
    va_start(vl, fmt);
    int sz = vsnprintf( 0, 0, fmt, vl );
    void *ptr = malloca( sz + 1 );
    vsnprintf( (char *)ptr, sz, fmt, vl );
    va_end(vl);
    return (char *)ptr;
}

#endif

#ifdef MALLOCA_DEMO
#include <stdio.h>
int main() {
    char *buf1 = mallocaf("hello world %d", 123);
    puts(buf1);

    char *buf2 = malloca(128);
    sprintf(buf2, "hello world %d", 123);
    puts(buf2);
}
#endif
