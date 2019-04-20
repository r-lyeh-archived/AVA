// ## string transform utils
// - rlyeh, public domain.

#ifndef TRIM_H
#define TRIM_H

/// - Search substring from beginning (left). Returns end-of-string if not found.
/// - Search substring from end (right). Returns end-of-string if not found.
API      const char*   strfindl(const char *string, const char *substring);
API      const char*   strfindr(const char *string, const char *substring);

/// ## String trimming
/// - Delete substring from a string.
/// - Trim characters from begin of string to first-find (`b-str-str-e` to `x-xxx-str-e`).
/// - Trim characters from begin of string to last-find (`b-str-str-e` to `x-xxx-xxx-e`).
/// - Trim characters from first-find to end of string (`b-str-str-e` to `b-xxx-xxx-x`).
/// - Trim characters from last-find to end of string (`b-str-str-e` to `b-str-xxx-x`).
/// - Trim leading, trailing and excess embedded whitespaces.
///<C
API      char*         strdel    (char *string, const char *substring);
API      char*         strtrimbff(char *string, const char *substring);
API      char*         strtrimblf(char *string, const char *substring);
API      char*         strtrimffe(char *string, const char *substring);
API      char*         strtrimlfe(char *string, const char *substring);
API      char*         strtrimws (char *string);

#endif

#ifdef TRIM_C
#pragma once
#include <ctype.h>
#include <string.h>
#include <stdlib.h>


const char* strfindl(const char *text, const char *substring) {
    const char *found = strstr( text, substring );
    return found ? found : text + strlen(text);
}
const char* strfindr(const char *text, const char *substring) {
    char *found = 0;
    while(1) {
        char *found2 = strstr(text, substring);
        if( !found2 ) break;
        found = found2;
        text = found2 + 1;
    }
    return found ? found : text + strlen(text);
}

char* strtrimbff(char *string, const char *substring) {
    char *found = strstr(string, substring);
    if( found ) {
        int L = strlen(substring);
        memmove(string, found+L, strlen(string) - L);
    }
    return string;
}
char* strtrimffe(char *string, const char *substring) {
    ((char *)strfindl(string, substring))[0] = 0;
    return string;
}
char* strtrimblf(char *string, const char *substring) {
    const char *found = strfindr(string, substring);
    int L = strlen(substring);
    memmove( string, found + L, strlen(found) - L + 1);
    return string;
}
char* strtrimlfe(char *string, const char *substring) {
    ((char *)strfindr(string, substring))[0] = 0;
    return string;
}
char *strtrimws(char *str) {
    char *ibuf, *obuf;
    if( str ) {
        for( ibuf = obuf = str; *ibuf; ) {
            while( *ibuf && isspace(*ibuf)  )  (ibuf++);
            if(    *ibuf && obuf != str     ) *(obuf++) = ' ';
            while( *ibuf && !isspace(*ibuf) ) *(obuf++) = *(ibuf++);
        }
        *obuf = 0;
    }
    return str;
}

char *strdel(char *string, const char *substring) {
    if( string ) {
        char *p = strstr(string, substring);
        if( p ) {
            for( int len = strlen(substring); p[len] ; ++p ) {
                p[0] = p[len];
            }
            *p = 0;
        }
    }
    return string;
}

#endif

#ifdef TRIM_DEMO
#include <stdio.h>
#include <assert.h>
#include <string.h>
int main() {
    {
        char bufws[] = "   Hello  world  ";
        assert( 0 == strcmp("Hello world", strtrimws(bufws)));

        char buf1[] = "hellohelloworldworld";
        char buf2[] = "hellohelloworldworld";
        char buf3[] = "hellohelloworldworld";
        char buf4[] = "hellohelloworldworld";
        strtrimbff(buf1, "ell"); puts(buf1); assert( 0 == strcmp( buf1, "ohelloworldworld") );
        strtrimffe(buf2, "ell"); puts(buf2); assert( 0 == strcmp( buf2, "h") );
        strtrimblf(buf3, "ell"); puts(buf3); assert( 0 == strcmp( buf3, "oworldworld") );
        strtrimlfe(buf4, "ell"); puts(buf4); assert( 0 == strcmp( buf4, "helloh") );
    }
    assert(~puts("Ok"));
}
#endif
