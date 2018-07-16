// vector based allocator (x1.75 enlarge factor)
// - rlyeh, public domain

#ifndef VALLOC_H
#define VALLOC_H
#include <stdio.h>

size_t vsize( void *p );
void *vrealloc( void *p, size_t sz );

#endif

#ifdef VALLOC_C
#pragma once
#include <stdlib.h>

size_t vsize( void *p ) {
    return p ? 0[ (size_t*)p - 2 ] : 0;
}

void *vrealloc( void *p, size_t sz ) {
    size_t *ret = (size_t*)p - 2;
    if( sz ) {
        if( !p ) {
            ret = malloc( sizeof(size_t) * 2 + sz );
            ret[0] = sz;
            ret[1] = 0;
        } else {
            size_t osz = ret[0];
            size_t ocp = ret[1];
            if( sz <= (osz + ocp) ) {
                ret[0] = sz;
                ret[1] = ocp - (sz - osz);
            } else {
                ret = realloc( ret, sizeof(size_t) * 2 + sz * 1.75 );
                ret[0] = sz;
                ret[1] = (size_t)(sz * 1.75) - sz;
            }
        }
        return &ret[2];
    } else {
        if( p ) {
            ret[0] = 0;
            ret[1] = 0;
            free( ret );
        }
        return 0;
    }
}

#endif
