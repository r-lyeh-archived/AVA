#include <stdarg.h>

/// ## String format (temporary buffers)
/// - Format a C-style formatted string. Returns temporary buffer (do not `free()`).
/// - Format a C-style formatted valist. Returns temporary buffer (do not `free()`).
/// - Note: 16 buffers are handled internally so that nested calls are safe within reasonable limits.
///<C
// API TEMP char *        strf (const char *format, ...);
// API TEMP char *        strfv(const char *format, va_list list);

/// ## String format (heap buffers)
/// - Assign a C-style formatted string. Reallocates input buffer (will create buffer if `str` is NULL).
/// - Assign a C-style formatted valist. Reallocates input buffer (will create buffer if `str` is NULL).
/// - Concat a C-style formatted string. Reallocates input buffer (will create buffer if `str` is NULL).
/// - Concat a C-style formatted valist. Reallocates input buffer (will create buffer if `str` is NULL).
///<C
API HEAP char *        strcpyf (char **string, const char *format, ...);
API HEAP char *        strcpyfv(char **string, const char *format, va_list list);
API HEAP char *        strcatf (char **string, const char *format, ...);
API HEAP char *        strcatfv(char **string, const char *format, va_list list);


#ifdef FORMAT_C
#pragma once

// ## C-style formatting
// Wrap vsnprintf into a statically allocated buffer. A bunch of buffers are
// handled internally so that nested calls are safe within reasonable limits.
// - rlyeh, public domain.

#ifndef STRF_MAXBUFFERS
#define STRF_MAXBUFFERS 16
#endif

#if 0
static THREAD_LOCAL int vl_index = 0;
static THREAD_LOCAL char *vl_buffer[STRF_MAXBUFFERS] = {0};
TEMP char *strv( const char *fmt, va_list lst ) {
    int idx = (++vl_index) % STRF_MAXBUFFERS;
    int sz = 1 + VSNPRINTF(0, 0, fmt, lst);
    vl_buffer[idx] = (char *)REALLOC( vl_buffer[idx], sz );
    VSNPRINTF( vl_buffer[idx], sz, fmt, lst );
    return vl_buffer[idx];
}
TEMP char *strf( const char *fmt, ... ) {
    va_list lst;
    va_start(lst, fmt);
    char *rc = strv(fmt, lst);
    va_end(lst);
    return rc;
}
#else
#define strv vl
#define strf va
#endif

HEAP char *strcpyv( char **str, const char *fmt, va_list lst ) {
    TEMP char *buf = strv( fmt, lst );
    if( str && *str ) {
        int len = strlen(buf) + 1;
        (*str) = (char*)REALLOC( str && (*str) ? (*str) : 0, len );
        memcpy( (*str), buf, len );
        return *str;
    } else {
        return str ? *str = STRDUP(buf) : STRDUP(buf);
    }
}
HEAP char *strcpyf( char **str, const char *fmt, ... ) {
    va_list lst;
    va_start(lst, fmt);
    char *rc = strcpyv(str, fmt, lst);
    va_end(lst);
    return rc;
}
HEAP char *strcatv( char **str, const char *fmt, va_list lst ) {
    TEMP char *buf = strv( fmt, lst );
    if( str && *str ) {
        int l1 = strlen(*str), l2 = strlen(buf);
        (*str) = (char*)REALLOC( (*str), l1 + l2 + 1 );
        memcpy( (*str) + l1, buf, l2 + 1 );
        return *str;
    } else {
        return str ? *str = STRDUP(buf) : STRDUP(buf);
    }
}
HEAP char *strcatf( char **str, const char *fmt, ... ) {
    va_list lst;
    va_start(lst, fmt);
    HEAP char *rc = strcatv(str, fmt, lst);
    va_end(lst);
    return rc;
}

#endif
