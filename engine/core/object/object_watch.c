#ifndef WATCH_H
#define WATCH_H

API void* watch( void *ptr, int sz );
API void* forget( void *ptr );

#endif

#ifdef WATCH_C
#pragma once
#include "object_realloc.c"
#include <stdlib.h>
#include <stdio.h>

// API char *callstack(int traces);

void* watch( void *ptr, int sz ) {
    if( ptr ) {
        char buf[256];
        sprintf(buf, "%p.mem", ptr);

        char *cs = callstack( +48 );
        FILE *fp = fopen(buf, "a+b");
        if( fp ) {
            fprintf(fp, "Memleak, built %s %s\n", __DATE__, __TIME__); // today() instead?
            fprintf(fp, "Pointer: [%p], size: %d\n%s", ptr, sz, cs ? cs : "");
            fclose(fp);
        }
        FREE(cs);
    }
    return ptr;
}
void* forget( void *ptr ) {
    if( ptr ) {
        char buf[256];
        sprintf(buf, "%p.mem", ptr);
        unlink(buf);
    }
    return ptr;
}
#endif

#ifdef WATCH_DEMO
#define malloc(p) watch(malloc(p))
#define free(p) free(forget(p))
#define realloc(p,sz) watch(realloc( forget(p), sz ))
int main() {
    void *p = malloc(101); // allocate
    p = realloc(p, 404);   // enlarge
    free(p);               // uncomment for leak detection
}
#endif
