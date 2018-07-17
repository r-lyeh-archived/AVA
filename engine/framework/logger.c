#pragma once
#include <stdio.h>

#ifndef LOG
#define LOG(tags, ...) ( \
    fprintf(stderr, __VA_ARGS__ ), \
    fprintf(stderr, " (%s:%d) #%s\n", __FILE__, __LINE__, #tags), \
    fprintf(stderr, "%s", 1[#__VA_ARGS__] == '!' && callstack_handler ? callstack_handler(+16) : "" ) \
)
#endif

#ifndef LOGLEVEL
#define LOGLEVEL 4
#endif

#define LOGEXTRA(tags, ...)   (void)0
#define LOGDEBUG(tags, ...)   (void)0
#define LOGINFO(tags, ...)    (void)0
#define LOGWARNING(tags, ...) (void)0
#define LOGERROR(tags, ...)   (void)0

// ---

#if LOGLEVEL >= 5
#undef  LOGEXTRA
#define LOGEXTRA LOG
#endif

#if LOGLEVEL >= 4
#undef  LOGDEBUG
#define LOGDEBUG LOG
#endif

#if LOGLEVEL >= 3
#undef  LOGINFO
#define LOGINFO LOG
#endif

#if LOGLEVEL >= 2
#undef  LOGWARNING
#define LOGWARNING LOG
#endif

#if LOGLEVEL >= 1
#undef  LOGERROR
#define LOGERROR LOG
#endif

// extra tip, redefinable:

extern const char *(*callstack_handler)(int traces); // = callstack;

// extra tools

#define FIXME(...) do { \
    static int seen = 0; \
    if( !seen ) { \
        seen = 1; \
        LOGDEBUG(FIXME, __VA_ARGS__); \
    } \
} while(0)

#define TODO(...) do { \
    static int seen = 0; \
    if( !seen ) { \
        seen = 1; \
        LOGDEBUG(TODO, __VA_ARGS__); \
    } \
} while(0)

