#ifndef FORMAT_H
#define FORMAT_H
#include <stdarg.h>

char *formatv( const char *fmt, va_list lst );
char *format( const char *fmt, ... );

#endif

#ifdef FORMAT_C
#pragma once
#include "realloc.c"

#if defined _MSC_VER && !defined __thread
#define __thread __declspec(thread)
#endif

#ifndef VSNPRINTF
#define VSNPRINTF vsnprintf
#endif

static __thread int vl_index = 0;
static __thread char *vl_buffer[16] = {0};

char *formatv( const char *fmt, va_list lst ) {
    int idx = (++vl_index) % 16;
    int sz = 1 + VSNPRINTF(0, 0, fmt, lst);
    vl_buffer[idx] = (char *)REALLOC( vl_buffer[idx], sz );
    VSNPRINTF( vl_buffer[idx], sz, fmt, lst );
    return vl_buffer[idx];
}
char *format( const char *fmt, ... ) {
    va_list lst;
    va_start(lst, fmt);
    char *rc = formatv(fmt, lst);
    va_end(lst);
    return rc;
}

#endif

#ifdef FORMAT_DEMO
#include <stdio.h>
int main() {
    puts( format("hello %s", format("%s%s", "wo", "rld")));
}
#endif
