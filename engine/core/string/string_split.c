/// ## String splitting
/// - Check delimiters and split string into tokens. Function never allocates.
/// - Check delimiters and split string into tokens. Returns null-terminated list.
/// - Join tokens into a string.
///<C
API      int           strchop (const char *string, const char *delimiters, int avail, const char *tokens[]);
API HEAP char**        strsplit(const char *string, const char *delimiters);
API HEAP char*         strjoin (char **out, const char *tokens[], const char *separator);

// ## split strings into tokens
// - rlyeh, public domain.

#ifdef SPLIT_C
#pragma once
#include "string.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>

int strchop( const char *string, const char *delimiters, int avail, const char *tokens[] ) { $
    assert( avail >= 4 && 0 == ( avail % 2 ) );
    for( avail /= 2; *string && avail-- > 0; ) {
        int n = strcspn( string += strspn(string, delimiters), delimiters );
        *tokens++ = (*tokens++ = (const char *)(uintptr_t)n) ? string : "";
        string += n;
    }
    return *tokens++ = 0, *tokens = 0, avail > 0;
}

HEAP char **strsplit( const char *string, const char *delimiters ) { $
    int L = strlen(string), len = sizeof(char *) * (L/2+1+1), i = 0;
    char **res = (char **)CALLOC(1, len + L + 1 );
    char *buf = strcpy( (char *)res + len, string );
    for( char *token = strtok(buf, delimiters); token; token = strtok(NULL, delimiters) ) {
        res[i++] = token;
    }
    return res;
}

HEAP char* strjoin( char **string, const char *words[], const char *separator ) {
    char *(*table[2])( char **, const char *, ... ) = { strcpyf, strcatf };
    char *out = string ? *string : 0;
    for( int i = 0; words[i] /*i < nwords*/; ++i ) {
        table[!!i]( &out, "%s%s", i > 0 ? separator : "", words[i] );
    }
    return string ? *string = out : out;
}

#endif

#ifdef SPLIT_DEMO
#include <stdio.h>
int main() {
    // split input text into tokens. allocates once.
    HEAP char **tokens = strsplit("JAN,;,FEB,MAR,APR,MAY,JUN,JUL,AUG,SEP,OCT,NOV,DEC", ",;");
    HEAP char *joint = strjoin( 0, tokens, "/" );
    puts( joint );
    free( joint );
    free( tokens );

    // split input text into tokens. does not allocate.
    const char *words[32] = {0};
    if( strchop( "There is a lady who's sure, all that glitter is gold...", " ,.", 32, words ) ) { // "$"
        for( int i = 0; words[i]; i += 2 ) {
            printf("[%.*s],", (int)(uintptr_t)words[i+0], words[i+1]);
        }
    }
}
#endif
