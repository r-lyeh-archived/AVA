// string utilities
// - rlyeh, public domain.

API array(char*) string_split(char *s, const char *delimiters);
API int          string_begins(const char *s, const char *b);
API int          string_ends(const char *s, const char *e);
API float        string_float(const char *s);
API unsigned     string_unsigned(const char *s);

// inliners
API char*        string_trim(char *s);
API char*        string_upper(char *s);
API char*        string_lower(char *s);
API char*        string_replace(char *s, const char *src, const char *dst); // replace only if dst shorter than src


#ifdef UTILITIES_C
#pragma once

array(char*) string_split(char *text, const char *delimiters) {
    array(char *) out = 0;

#if 0
    for( char *token = strtok(text, delimiters); token; token = strtok(NULL, delimiters) ) {
        array_push(out, token);
    }
#else
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
#endif

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

char *string_replace( char *copy, const char *target, const char *replacement ) { // replace only if new text is shorter than old one
    int rlen = strlen(replacement), diff = strlen(target) - rlen;
    if( diff >= 0 ) {
        for( char *s = copy, *e = s + strlen(copy); /*s < e &&*/ 0 != (s = strstr(s, target)); ) {
            if( rlen ) s = (char*)memcpy( s, replacement, rlen ) + rlen;
            if( diff ) memmove( s, s + diff, (e - (s + diff)) + 1 );
        }
    }
    return copy;
}

float string_float( const char *s ) {
    return (float)atof(s);
}
unsigned string_unsigned( const char *s ) {
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

// trims leading, trailing and excess embedded whitespaces.
char* string_trim(char *str) {
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

#if 0
// joins tokens into a string.
string string_join( char **string, const char *words[], const char *separator ) {
    char *(*table[2])( char **, const char *, ... ) = { strcpyf, strcatf };
    char *out = string ? *string : 0;
    for( int i = 0; words[i] /*i < nwords*/; ++i ) {
        table[!!i]( &out, "%s%s", i > 0 ? separator : "", words[i] );
    }
    return string ? *string = out : out;
}
#ifdef SPLIT_DEMO
#include <stdio.h>
int main() {
    // split input text into tokens. allocates once.
    array(string) tokens = string_split("JAN,;,FEB,MAR,APR,MAY,JUN,JUL,AUG,SEP,OCT,NOV,DEC", ",;");
    string joint = string_join( tokens, "/" );
    puts( joint );
    string_free( joint );
    array_free( tokens );
}
#endif
#endif


#ifdef UTILITIES_DEMO
int main() {
    char bufws[] = "   Hello  world  ";
    assert( 0 == strcmp("Hello world", string_trim(bufws)));

    assert( !strcmp( string_replace(strdup("hello world"), "world", "123"), "hello 123" ));
    assert( !strcmp( string_replace(strdup("hello world"), "world", "-> 123"), "hello world" ));

    assert( !strcmp( string_replace(strdup("abracadabra"), "bra", "bra"), "abracadabra" )); // same len
    assert( !strcmp( string_replace(strdup("abracadabra"), "bra", "br"), "abrcadabr" ));    // smaller len
    assert( !strcmp( string_replace(strdup("abracadabra"), "bra", "b"), "abcadab" ));       // samller len
    assert( !strcmp( string_replace(strdup("abracadabra"), "bra", ""), "acada" ));          // erase
    assert(~puts("ok"));
}
#endif
#endif
