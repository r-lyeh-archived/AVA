/// ## String regular expression
/// - Regular expression matching. returns non-zero if found.
///   - `c` matches any literal character c.
///   - `?` matches any single character.
///   - `^` matches the beginning of the input string.
///   - `$` matches the end of the input string.
///   - `*` matches zero or more occurrences of the previous character.
/// - Return true if string matches wildcard pattern expression (?*).
///<C

API      int           strregex(const char *string, const char *regex);
API      bool          strmatch(const char *string, const char *substring);

#ifdef REGEX_C
#pragma once

// ## rob pike's regexp match (apparently public domain).
// [ref] https://www.cs.princeton.edu/courses/archive/spr09/cos333/beautiful.html
// @todo: evaluate @kokke/tiny-regex-c instead.

int regstar(const char *string, const char *re, int c);

int reghere(const char *string, const char *re) {
    if( re[0] == '\0' ) return 1;
    if( re[1] == '*' ) return regstar(string, re+2, re[0]);
    if( re[0] == '$' && re[1] == '\0' ) return *string == '\0';
    if( *string!='\0' && (re[0]=='?' || re[0]==*string) ) return reghere(string+1, re+1);
    return 0;
}

int regstar(const char *string, const char *re, int c) {
    do { /* a * matches zero or more instances */
        if( reghere(string, re) ) return 1;
    } while( *string != '\0' && (*string++ == c || c == '?') );
    return 0;
}

int strregex(const char *string, const char *re) {
    if( re[0] == '^' ) return reghere(string, re+1);
    do { /* must look even if string is empty */
        if( reghere(string, re) ) return 1;
    } while (*string++ != '\0');
    return 0;
}

bool strmatch( const char *text, const char *pattern ) { $
    if( *pattern=='\0' ) return !*text;
    if( *pattern=='*' )  return strmatch(text, pattern+1) || (*text && strmatch(text+1, pattern));
    if( *pattern=='?' )  return *text && (*text != '.') && strmatch(text+1, pattern+1);
    return (*text == *pattern) && strmatch(text+1, pattern+1);
}

#endif

#ifdef REGEX_DEMO
#include <assert.h>
#include <stdio.h>
int main() {
    assert( strregex("hello123", "^hel?*$") );
    assert( strmatch("hello", "h?ll*") );
    assert( ~puts("Ok.") );
}
#endif