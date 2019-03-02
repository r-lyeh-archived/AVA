// vector based allocator (x1.75 enlarge factor)
// - rlyeh, public domain

#ifndef VREALLOC_H
#define VREALLOC_H
#include <stdio.h>

size_t vsize( void *p );
void *vrealloc( void *p, size_t sz );
void *vresize( void *p, size_t sz );

#endif

#ifdef VREALLOC_C
#pragma once
#include "../memory/memory_realloc.c"
#include <stdlib.h>

size_t vsize( void *p ) {
    return p ? 0[ (size_t*)p - 2 ] : 0;
}

void *vresize( void *p, size_t sz ) {
    size_t *ret = (size_t*)p - 2;
    if( !p ) {
        ret = REALLOC( 0, sizeof(size_t) * 2 + sz );
        ret[0] = sz;
        ret[1] = 0;
    } else {
        size_t osz = ret[0];
        size_t ocp = ret[1];
        if( sz <= (osz + ocp) ) {
            ret[0] = sz;
            ret[1] = ocp - (sz - osz);
        } else {
            ret = REALLOC( ret, sizeof(size_t) * 2 + sz * 1.75 );
            ret[0] = sz;
            ret[1] = (size_t)(sz * 1.75) - sz;
        }
    }
    return &ret[2];
}

void *vrealloc( void *p, size_t sz ) {
    if( sz ) {
        return vresize( p, sz );
    } else {
        if( p ) {
            size_t *ret = (size_t*)p - 2;
            ret[0] = 0;
            ret[1] = 0;
            REALLOC( ret, 0 );
        }
        return 0;
    }
}

#endif
