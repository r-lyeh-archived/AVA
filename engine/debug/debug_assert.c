// # asserts ##################################################################

#ifndef ASSERT_H
#define ASSERT_H
#include <stdio.h>

API void panic( const char *description );

#if SHIPPING
#define ASSERT(EXPR, ...)
#else
#define ASSERT(EXPR, ...) do { \
    int msvc_trick[] = {0,}; \
    if( !(EXPR) ) { \
        static int once = 1; \
        for( ; once ; once = 0 ) { \
            char text[4096], *ptr = text; \
            ptr += sprintf( ptr, "!" __VA_ARGS__ ); \
            ptr += text[1] ? 0 : sprintf( ptr, "%s", #EXPR ); \
            ptr += sprintf( ptr, "\n(assertion failed)\n%s:%d", __FILE__, __LINE__); \
            panic( text ); \
        } \
    } } while(0)
#endif

#endif

#ifdef ASSERT_DEMO
#pragma once
int main() {
	ASSERT(1 < 2);
	ASSERT(1 > 2, "Assert demo: %d bigger than %d?", 1, 2);
}
#endif


/*
#ifndef ASSERTLEVEL
#define ASSERTLEVEL 100 // percentage of random checked asserts: [0%(none)..50%(half)..100%(all)]
#endif

#ifndef ASSERT
#define ASSERT(expr) do { \
    if( ((STRHASH(STRINGIZE(__LINE__)) % 100) < (ASSERTLEVEL)) && !(EXPR) ) ASSERT(!#expr); \
} while(0)
*/
