// string object type.
// - rlyeh, public domain.

#ifndef STRING3_H
#define STRING3_H

// api

typedef array(char) string;

API string string_alloc( const char *fmt, ... );
API void   string_append( string *s, const char *fmt, ... );
API int    string_count( string s );
API void   string_free( string *s );

// syntax sugars

#define string_append(s, ...) string_append(&(s), __VA_ARGS__)
#define string_free(s, ...)   string_free(&(s))

#endif

// ----------------------------------------------------------------------------

#ifdef STRING3_C
#pragma once
#include <stdlib.h>
#include <stdarg.h>
#define STB_SPRINTF_IMPLEMENTATION
#include "3rd/stb_sprintf.c"

string string_alloc( const char *fmt, ... ) {
    va_list vl;
    va_start(vl, fmt);
    int len = stbsp_vsnprintf(0, 0, fmt, vl) + 1;
    string s = 0;
    array_resize(s, len);
    stbsp_vsnprintf(s, len, fmt, vl);
    va_end(vl);

    return s;
}
void (string_append)( string *s, const char *fmt, ... ) {
    va_list vl;
    va_start(vl, fmt);
    int len = stbsp_vsnprintf(0, 0, fmt, vl) + 1, slen = string_count(*s);
    array_resize(*s, slen + len); s[slen+len] = 0;
    stbsp_vsnprintf(*s + slen, len, fmt, vl);
    va_end(vl);
}
void (string_free)( string *s ) {
    if(*s) array_free(*s);
    *s = 0;
}
int string_count( string s ) {
    return s ? array_count(s) - 1 : 0;
}

// ----------------------------------------------------------------------------

#ifdef STRING3_DEMO
#include <stdio.h>
int main() {
    string s = string_alloc("hello %s", "world");
    printf("strlen: %d : %s\n", string_count(s), s);

    string_append( s, "%s %d", s, 12345 );
    printf("strlen: %d : %s\n", string_count(s), s);

    string_free(s);
    printf("strlen: %d : %p\n", string_count(s), s);
}
#endif
#endif

