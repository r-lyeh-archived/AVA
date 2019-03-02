#ifndef CMP_H
#define CMP_H

API int cmpi(const void *a, const void *b );
API int cmpu(const void *a, const void *b );
API int cmps(const void *a, const void *b );

API int         cmp64i      ( const void *a, const void *b );
API int         cmp64u      ( const void *a, const void *b );
API int         cmp64f      ( const void *a, const void *b );
API int         cmp32i      ( const void *a, const void *b );
API int         cmp32u      ( const void *a, const void *b );
API int         cmp32f      ( const void *a, const void *b );
API int         cmp16i      ( const void *a, const void *b );
API int         cmp16u      ( const void *a, const void *b );
API int         cmpptr      ( const void *a, const void *b );
API int         cmpstr      ( const void *a, const void *b );
API int         cmpstri     ( const void *p, const void *q );

#endif


#ifdef CMP_C
#pragma once
#include <stdint.h>
#include <string.h>

int cmpi(const void *a, const void *b ) { return ( *((      int*)a) - *((      int*)b) ); }
int cmpu(const void *a, const void *b ) { return ( *(( unsigned*)a) - *(( unsigned*)b) ); }
int cmps(const void *a, const void *b ) { return   strcmp((const char*)a,(const char*)b); }

int cmp64i(const void *a, const void *b ) { return ( *((  int64_t*)a) - *((  int64_t*)b) ); }
int cmp64u(const void *a, const void *b ) { return ( *(( uint64_t*)a) - *(( uint64_t*)b) ); }
int cmp64f(const void *a, const void *b ) { return ( *((   double*)a) - *((   double*)b) ); }
int cmp32i(const void *a, const void *b ) { return ( *((  int32_t*)a) - *((  int32_t*)b) ); }
int cmp32u(const void *a, const void *b ) { return ( *(( uint32_t*)a) - *(( uint32_t*)b) ); }
int cmp32f(const void *a, const void *b ) { return ( *((    float*)a) - *((    float*)b) ); }
int cmp16i(const void *a, const void *b ) { return ( *((  int16_t*)a) - *((  int16_t*)b) ); }
int cmp16u(const void *a, const void *b ) { return ( *(( uint16_t*)a) - *(( uint16_t*)b) ); }
int cmpptr(const void *a, const void *b ) { return ( *((uintptr_t*)a) - *((uintptr_t*)b) ); } // return a<b?-1:(a>b?1:0);

int cmpstr(const void *a, const void *b ) { return cmps(a,b); }
int cmpstri(const void *p, const void *q) {
    const char *a = (const char *)p, *b = (const char *)q;
    for( ; *a && *b ; ++a, ++b ) {
        if( ((*a)|32) != ((*b)|32)) {
            break;
        }
    }
    return (*a) - (*b);
}

#endif

#ifdef CMP_DEMO
#include <assert.h>
#include <stdio.h>
int main() {
    uint64_t a64 = 0, b64 = 0;
    assert( 0 == cmp64u(&a64, &b64));
    const char *astr = "hello", *bstr = "Hello";
    assert( 0 != cmpstr(astr, bstr));
    assert( 0 == cmpstri(astr, bstr));
    assert(~puts("Ok"));
}
#endif
