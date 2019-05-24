// string object type.
// - rlyeh, public domain.

#ifndef STRING2_H
#define STRING2_H

typedef char* string;

// temp stack
API string string_local( const char *fmt, ... );

// heap
API string string_alloc( const char *fmt, ... );
API string string_push( string* s, string a );
API void   string_free( string* s );

// common
API int    string_count( string s );
API int    string_hash( string s );
API int    string_compare( string a, string b );

// syntax sugars
#define string_push(s, ...)   string_push(&(s), string_local(__VA_ARGS__))
#define string_free(s)        string_free(&(s))

#endif

// ----------------------------------------------------------------------------

#ifdef STRING2_C
#pragma once
#define STB_SPRINTF_IMPLEMENTATION
#include "3rd/stb_sprintf.c"
#define VSNPRINTF_ stbsp_vsnprintf

// temp stack

char *string_local( const char *fmt, ... ) {
    static THREAD_LOCAL char vl_buf[2048];
    static THREAD_LOCAL int  vl_idx = 0;

    va_list v;
    va_start(v, fmt);
        int extra = 4;
        int l = 1+VSNPRINTF_(0, 0, fmt, v )+extra;
            assert(l <= 2048);
            if( (vl_idx + l) >= 2048 ) vl_idx = 0;
            char *s = vl_buf + vl_idx + extra;
            VSNPRINTF_(s, 2048-1-vl_idx, fmt, v );
            vl_idx += l;
    va_end(v);

    uint64_t hash = 0; // fnv1a: 14695981039346656037ULL;
    for( int i = 0; i < extra; ++i ) { hash = ( s[i] ^ hash ) * 131; } // fnv1a: 0x100000001b3ULL; }
    union { uint64_t u; struct { uint8_t a,b,c,d,e,f,g,h; }; } x; x.u = hash;
    extra |= (x.a ^ x.b ^ x.c ^ x.d ^ x.e ^ x.f ^ x.g ^ x.h) << 24;

    *(int32_t*)(&s[-4]) = l-1-extra;
    return s;
}

// heap

string string_alloc( const char *fmt, ... ) {
    va_list v;
    va_start(v, fmt);
        int extra = 4;
        int l = 1+VSNPRINTF_(0, 0, fmt, v )+extra;
            array(char) s = 0;
            array_resize(s, l); s += extra;
            VSNPRINTF_(s, l-1, fmt, v );
    va_end(v);

    uint64_t hash = 0; // fnv1a: 14695981039346656037ULL;
    for( int i = 0; i < extra; ++i ) { hash = ( s[i] ^ hash ) * 131; } // fnv1a: 0x100000001b3ULL; }
    union { uint64_t u; struct { uint8_t a,b,c,d,e,f,g,h; }; } x; x.u = hash;
    extra |= (x.a ^ x.b ^ x.c ^ x.d ^ x.e ^ x.f ^ x.g ^ x.h) << 24;

    *(int32_t*)(&s[-4]) = l-1-extra;
    return s;
}
string (string_push)( string *s, string t ) {
    int slen = string_count(*s), len = string_count(t);
    char *ptr = &(*s)[-4];
    array_resize(ptr, slen + len);
    return (*s = &ptr[4], memcpy(*s + slen, t, len+1), *s);
}
void (string_free)( string* s ) {
    if( s ) {
        char *ptr = &(*s)[-4];
        if( *s ) array_free( ptr );
        *s = 0;
    }
}

// common

int string_hash( string s ) {
    return *(uint32_t*)(&s[-4]);
}

int string_count( string s ) {
    return ( (uint32_t)string_hash(s) ) & 0x00FFFFFF;
}

int string_compare( string a, string b ) {
    uint32_t ha = string_hash(a), hb = string_hash(b);
    return ha == hb ? strcmp(a,b) : (hb & 0x00FFFFFF) - (ha & 0x00FFFFFF); // strncmp(a,b,len)
}

// ----------------------------------------------------------------------------

#ifdef STRING2_DEMO
int main() {
    // temp
    string s = string_local("hello world %d", 123);
    printf("%s (%d chars) (%x hash) (%p local)\n", s, string_count(s), string_hash(s), s);

    // heap
    string p = string_alloc("hello world %d", 123);
    printf("%s (%d chars) (%x hash) (%p alloc)\n", p, string_count(p), string_hash(p), p);

    // compare
    printf("%x[%p] == %x[%p] ? -> %d\n", string_hash(s),s, string_hash(p),p, string_compare(s,p));

    // utils
    string_push(p, "+%s:%d", p, 12345 );
    puts(p);

    string_free(p);
    puts("ok");
}
#endif
#endif
