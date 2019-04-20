// string

/*
typedef char* string;
typedef const char* quark;
*/

#ifndef STRING2_H
#define STRING2_H

API array(char*) string_split(char *text, const char *delimiters);
API char *string_upper( char *copy );
API char *string_lower( char *copy );
API char *string_replace_inline( char *copy, const char *target, const char *replacement ); // replace only if new text is shorter than old one
API float string_tofloat( const char *s );
API unsigned string_tounsigned( const char *s );
API int string_begins( const char *string, const char *substr );
API int string_ends( const char *s, const char *e );

#endif

#ifdef STRING2_C
#pragma once
//#include <engine.h>

array(char*) string_split(char *text, const char *delimiters) {
    array(char *) out = 0;

    int found[256] = {1,0}, i = 0;
    while( *delimiters ) found[(unsigned char)*delimiters++] = 1;

    while( text[i] ) {
        int begin = i; while(text[i] && !found[(unsigned char)text[i]]) ++i;
        int end = i;   while(text[i] &&  found[(unsigned char)text[i]]) ++i;
        if (end > begin) {
            array_push(out, (text + begin));
            (text + begin)[ end - begin ] = 0;
        }
    }
    return out;
}

char *string_upper( char *copy ) {
    for( char *s = copy; *s; ++s ) if( *s >= 'a' && *s <= 'z' ) *s = *s - 'a' + 'A';
    return copy;
}

char *string_lower( char *copy ) {
    for( char *s = copy; *s; ++s ) if( *s >= 'A' && *s <= 'Z' ) *s = *s - 'A' + 'a';
    return copy;
}

#include <string.h>
char *string_replace_inline( char *copy, const char *target, const char *replacement ) { // replace only if new text is shorter than old one
    int rlen = strlen(replacement), diff = strlen(target) - rlen;
    if( diff >= 0 ) {
        for( char *s = copy, *e = s + strlen(copy); /*s < e &&*/ 0 != (s = strstr(s, target)); ) {
            if( rlen ) s = (char*)memcpy( s, replacement, rlen ) + rlen;
            if( diff ) memmove( s, s + diff, (e - (s + diff)) + 1 );
        }
    }
    return copy;
}
/*
#include <assert.h>
int main() {
    assert( !strcmp( replace_inline(strdup("hello world"), "world", "123"), "hello 123" ));
    assert( !strcmp( replace_inline(strdup("hello world"), "world", "-> 123"), "hello world" ));

    assert( !strcmp( replace_inline(strdup("abracadabra"), "bra", "bra"), "abracadabra" )); // same len
    assert( !strcmp( replace_inline(strdup("abracadabra"), "bra", "br"), "abrcadabr" ));    // smaller len
    assert( !strcmp( replace_inline(strdup("abracadabra"), "bra", "b"), "abcadab" ));       // samller len
    assert( !strcmp( replace_inline(strdup("abracadabra"), "bra", ""), "acada" ));          // erase
    assert( puts("ok") >= 0 );
}
*/

float string_tofloat( const char *s ) {
    return (float)atof(s);
}
unsigned string_tounsigned( const char *s ) {
    return (unsigned)atoi(s);
}


// returns true if text starts with substring
int string_begins( const char *string, const char *substr ) {
    return strncmp(string, substr, strlen(substr)) == 0;
}

// returns true if text ends with substring
int string_ends( const char *s, const char *e ) {
    int ls = strlen(s);
    int le = strlen(e);
    if( ls < le ) return 0;
    return 0 == memcmp( s - le, e, le );
}

#endif
