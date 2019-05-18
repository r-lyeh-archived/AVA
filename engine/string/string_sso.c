// small string optimizations for 32bit (3chr) and 64bit (7chr).
// - rlyeh, public domain.

#ifndef SSO_H
#define SSO_H

typedef char sso3[4];

sso3 *sso3_new( sso3 *s, const char *text );
int sso3_capacity( const sso3 *s );
int sso3_length( const sso3 *s );
int sso3_compare( const sso3 *a, const sso3 *b );

//

typedef char sso7[8];

sso7 *sso7_new( sso7 *s, const char *text );
int sso7_capacity( const sso7 *s );
int sso7_length( const sso7 *s );
int sso7_compare( const sso7 *a, const sso7 *b );

#endif

// ----------------------------------------------------------------------------

#ifdef SSO_C
#pragma once

typedef int static_check_sso3[ sizeof(sso3) == (3+1) ];

sso3 *sso3_new( sso3 *s, const char *text ) {
    char *capacity = (char*)s + 3;
    int l = strlen(text);
    assert( l <= 3 );
    memcpy( s, text, l + 1 );
    *capacity = 3 - l;
    return s;
}

int sso3_capacity( const sso3 *s ) {
    char *capacity = (char*)s + 3;
    return *capacity;
}

int sso3_length( const sso3 *s ) {
    char *capacity = (char*)s + 3;
    return 3 - *capacity;
}

int sso3_compare( const sso3 *a, const sso3 *b ) {
    return *((uint32_t*)a) - *((uint32_t*)b);
}

//

typedef int static_check_sso7[ sizeof(sso7) == (7+1) ];

sso7 *sso7_new( sso7 *s, const char *text ) {
    char *capacity = (char*)s + 7;
    int l = strlen(text);
    assert( l <= 7 );
    memcpy( s, text, l + 1 );
    *capacity = 7 - l;
    return s;
}

int sso7_capacity( const sso7 *s ) {
    char *capacity = (char*)s + 7;
    return *capacity;
}

int sso7_length( const sso7 *s ) {
    char *capacity = (char*)s + 7;
    return 7 - *capacity;
}

int sso7_compare( const sso7 *a, const sso7 *b ) {
    return *((uint64_t*)a) - *((uint64_t*)b);
}

// ----------------------------------------------------------------------------

#ifdef SSO_DEMO
int main() {
    sso7 a;
    printf("(sizeof struct: %d)\n", (int)sizeof(sso7));

    sso7_new(&a, ""); // 0+1
    printf("(capacity: %d) (length: %d) (string: '%s')\n", sso7_capacity(&a), sso7_length(&a), &a);

    sso7_new(&a, "hello"); // 5+1
    printf("(capacity: %d) (length: %d) (string: '%s')\n", sso7_capacity(&a), sso7_length(&a), &a);

    sso7_new(&a, "hello!"); // 6+1
    printf("(capacity: %d) (length: %d) (string: '%s')\n", sso7_capacity(&a), sso7_length(&a), &a);

    sso7_new(&a, "hello!!"); // 7+1
    printf("(capacity: %d) (length: %d) (string: '%s')\n", sso7_capacity(&a), sso7_length(&a), &a);

    sso7 b = "abc";
    sso7 c = "acd";
    int compare1 = sso7_compare(&b, &c);
    printf("comparison '%s' %c '%s'\n", &b, compare1 < 0 ? '<' : (compare1 > 0 ? '>' : '='), &c);

    int compare2 = sso7_compare(&b, &b);
    printf("comparison '%s' %c '%s'\n", &b, compare2 < 0 ? '<' : (compare2 > 0 ? '>' : '='), &b);

    int compare3 = sso7_compare(&c, &b);
    printf("comparison '%s' %c '%s'\n", &c, compare3 < 0 ? '<' : (compare3 > 0 ? '>' : '='), &b);
}
#endif
#endif
