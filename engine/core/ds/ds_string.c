#ifndef STRING_H
#define STRING_H

typedef char *string[2];
char* string_new( string s, const char *fmt, ... );
char* string_cat( string s, const char *fmt, ... );
int   string_len( string s );
void  string_del( string s );

#endif

#ifdef STRING_C
#pragma once
#include <stdlib.h>
#include <stdarg.h>
#include "../object/object_realloc.c"

#define beg(s) (0[s])
#define end(s) (1[s])
#define cap(s) (2[s]) // unused for now
char* string_new( string s, const char *fmt, ... ) {
    va_list vl;
    va_start(vl, fmt);

    //strcpyfv( &beg(s), fmt, vl );
    int len = vsnprintf(0, 0, fmt, vl) + 1;
    beg(s) = (char*)REALLOC( 0, len );
    vsnprintf( beg(s), len, fmt, vl );

    va_end(vl);

    end(s) = beg(s) + strlen(beg(s));
    return beg(s);
}
char* string_cat( string s, const char *fmt, ... ) {
    va_list vl;
    va_start(vl, fmt);

    //strcatfv( &beg(s), fmt, vl );
    int len = vsnprintf(0, 0, fmt, vl) + 1, slen = string_len(s);
    beg(s) = (char*)REALLOC( beg(s), slen + len);
    vsnprintf(beg(s) + slen, len, fmt, vl);

    va_end(vl);

    end(s) = beg(s) + strlen(beg(s));
    return beg(s);
}
void string_del( string s ) {
    REALLOC(beg(s), 0);
    beg(s) = end(s) = /*cap(s) =*/ 0;
}
int string_len( string s ) {
    return end(s) - beg(s);
}

#endif

#ifdef STRING_DEMO
#include <stdio.h>
int main() {
    string s = {0};
    printf("strlen: %d\n", string_len(s));

    string_new( s, "'hello %s'", "world." );
    puts(*s);
    string_cat( s, "'hello %d'", 123 );
    puts(*s);

    printf("strlen: %d\n", string_len(s));
    string_del( s );

    printf("strlen: %d\n", string_len(s));
}
#endif
