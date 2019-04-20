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
    int l = VSNPRINTF(0, 0, fmt, v );
    assert(l >= 0);
    assert(l+1 <= 2048);
    char *dst = vl_buf + (vl_idx + l < 2048 ? vl_idx : 0);
    vl_idx += VSNPRINTF( dst, 2048, fmt, v ) + 1;
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
