#ifndef VA_H
#define VA_H
#include <stdarg.h>

API char *vl( const char *fmt, va_list vl );
API char *va( const char *fmt, ... );

#endif


#ifdef VA_C
#pragma once
#include <assert.h>
#ifndef VSNPRINTF
#define VSNPRINTF vsnprintf
#endif
static THREAD_LOCAL char vl_buf[2048];
static THREAD_LOCAL int  vl_idx = 0;
char *vl( const char *fmt, va_list v ) {
    int l = 1+VSNPRINTF(0, 0, fmt, v );
    assert(l >= 1);
    assert(l <= 2048);
    if( (vl_idx + l) >= 2048 ) vl_idx = 0;
    char *dst = vl_buf + vl_idx;
    vl_idx += 1+VSNPRINTF( dst, 2048-1-vl_idx, fmt, v );
    return dst;
}
char *va( const char *fmt, ... ) {
    va_list v;
    va_start(v, fmt);
    char *dst = vl( fmt, v );
    va_end(v);
    return dst;
}
#endif
