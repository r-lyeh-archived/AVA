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
    fprintf(stderr, "%s", 0[fmt] == '!' && logger_handle() ? (*logger_handle())(+16) : "" ) \
)
#endif

// tip: redefinable callstack handler
typedef const char *(*logger_callstack_handler)(int traces);
API logger_callstack_handler* logger_handle();

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

#if LOGLEVEL <= 0
#undef  LOG
#define LOG(tags, fmt, ...)
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

logger_callstack_handler callstack_handler = callstack;

logger_callstack_handler *logger_handle() {
    return &callstack_handler;
}

#endif


#ifdef LOGGER_DEMO
int main() {
    FIXME("write a better demo");
    LOG(AUDIO|ASSET, "!This is an audio test %d, with callstack", 123);
}
#endif
