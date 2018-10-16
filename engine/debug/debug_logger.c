#ifndef LOGGER_H
#define LOGGER_H
#include <stdio.h>

#ifndef LOGLEVEL
#define LOGLEVEL 4
#endif

#ifndef LOG
#define LOG(tags, fmt, ...) ( \
    fprintf(stderr, fmt, __VA_ARGS__ ), \
    fprintf(stderr, " (%s:%d) #%s\n", __FILE__, __LINE__, #tags), \
    fprintf(stderr, "%s", 0[fmt] == '!' && callstack_handler ? callstack_handler(+16) : "" ) \
)
#endif

// extra tip, redefinable:
extern API const char *(*callstack_handler)(int traces);

// ---

#define LOGEXTRA(tags, ...)   (void)0
#define LOGDEBUG(tags, ...)   (void)0
#define LOGINFO(tags, ...)    (void)0
#define LOGWARNING(tags, ...) (void)0
#define LOGERROR(tags, ...)   (void)0

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

// ---

// extra tools

#define FIXME(...) do { \
    static int seen = 0; \
    for( ; !seen; seen = 1 ) { \
        LOGDEBUG(FIXME, __VA_ARGS__); \
    } \
} while(0)

#define TODO(...) do { \
    static int seen = 0; \
    for( ; !seen; seen = 1 ) { \
        LOGDEBUG(TODO, __VA_ARGS__); \
    } \
} while(0)

#endif

#ifdef LOGGER_C
#pragma once
#include "../detect/detect_callstack.c"

const char *(*callstack_handler)(int traces) = callstack;
#endif


#ifdef LOGGER_DEMO
int main() {
    FIXME("write a better demo");
    LOG(AUDIO|ASSET, "!This is an audio test %d, with callstack", 123);
}
#endif
