//                     /\\ \// /\\
//                  small game engine
//
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.
//
// Dear contributor, check following directives before contributing:
//   - APIs follow an imperative, immediate mode design.
//   - Functions use the least possible arguments.
//   - Implementations use the simplest possible code.
//   - Style is readable and compact enough. Indent is 4-spaces.
//   - Your contributions are Zero-BSD, CC0 or Unlicensed.
//
// People who worked on this file: @r-lyeh,
//

#ifndef AVA_VERSION
#define AVA_VERSION "2018.3" // 'YOB-Marrow' release

// System defines here

#ifndef _ALLOW_KEYWORD_MACROS
#define _ALLOW_KEYWORD_MACROS // msc
#endif
#ifndef _BSD_SOURCE
#define _BSD_SOURCE
#endif
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS // msc
#endif
#ifndef _DEFAULT_SOURCE
#define _DEFAULT_SOURCE
#endif
#ifndef _FILE_OFFSET_BITS
#define _FILE_OFFSET_BITS 64
#endif
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#ifndef _LARGE_FILE
#define _LARGE_FILE 1
#endif
#ifndef _LARGEFILE_SOURCE
#define _LARGEFILE_SOURCE 1
#endif
#ifndef _LARGEFILE64_SOURCE
#define _LARGEFILE64_SOURCE
#endif
#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 199309L
#endif
#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES // msc
#endif
#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE 700
#endif
#ifndef NOMINMAX
#define NOMINMAX // win
#endif
#ifndef UNICODE
#define UNICODE // win
#endif
#ifndef _UNICODE
#define _UNICODE // win
#endif
#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN // msc
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN 1 // win
#endif

// OS includes here

#if _WIN32
//#include <winsock2.h>
//#include <io.h>
#else
#include <unistd.h>
#endif

// C includes here

#include <assert.h>
#include <math.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// api macros

#  if !defined(API) && defined(__cplusplus)
#define API extern "C" LINKAGE
#elif !defined(API)
#define API            LINKAGE
#endif

#ifndef DLL
#define DLL(name, version, ...) __VA_ARGS__
#endif

// symbol linkage and visibility

#ifndef EXPORT
#define EXPORT __declspec(dllexport)
#endif

#ifndef IMPORT
#define IMPORT __declspec(dllimport)
#endif

#ifndef STATIC
#define STATIC
#endif

#ifndef LINKAGE
#define LINKAGE STATIC
#endif

// code annotations

#define HEAP           // heap pointer. must free() after use
#define TEMP           // temporary stack pointer. do not free() after use
#define GC             // garbage collected pointer. do not free() after use
#define FAT            // fat pointer (internal data at &[-1])

// detect platform
#if 0
#define AND 0
#define BSD 0
#define IOS 0
#define LIN 0
#define OSX 0
#define PS4 0
#define RPI 0
#define STK 0
#define UNX 0
#define WEB 0
#define WIN 0
#define XB1 0

#  if defined __EMSCRIPTEN__
#undef  WEB
#define WEB           1
#elif defined _XBOX_ONE || defined _DURANGO
#undef  XB1
#define XB1           1 // before windows
#elif defined _WIN32 || defined __MINGW32__
#undef  WIN
#define WIN           1
#elif defined __ANDROID__
#undef  AND
#define AND           1 // before __linux__
#elif defined __ORBIS__
#undef  PS4
#define PS4           1 // before __linux__
#elif defined __STEAMLINK__
#undef  STK
#define STK           1 // before __linux__
#elif defined __VCCOREVER__
#undef  RPI
#define RPI           1 // before __linux__
#elif defined __linux__
#undef  LIN
#define LIN           1 // before __unix__
#elif defined __ENVIRONMENT_IPHONE_OS_VERSION_MIN_REQUIRED__ || defined __ENVIRONMENT_TV_OS_VERSION_MIN_REQUIRED__
#undef  IOS
#define IOS           1 // before bsd
#elif defined __ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__ || defined __APPLE__
#undef  OSX
#define OSX           1 // before bsd
#elif defined __NetBSD__ || defined __FreeBSD__ || defined __OpenBSD__ || defined __DragonFly__
#undef  BSD
#define BSD           1 // before __unix__
#elif defined __unix__
#undef  UNX
#define UNX           1
#endif

// detect compiler

#define MSC 0
#define GCC 0
#define CLANG 0

#  if defined _MSC_VER
#undef  MSC
#define MSC 1
#elif defined __clang__
#undef  CLANG
#define CLANG 1
#elif defined __GNUC__
#undef  GCC
#define GCC 1
#endif

// compile-time IF/IFN build macros

#define IF(x, true_, ...)  IF_(x, true_, __VA_ARGS__)
#define IFN(x, true_, ...) IF_(x, __VA_ARGS__, true_)
#define IF_(x, true_, ...) IF_##x(true_, __VA_ARGS__)
#define IF_0(true_,...)    __VA_ARGS__
#define IF_1(true_,...)    true_
#else
#include "core/detect.c"
#endif

// tags

#define TAG4(abcd) (((#abcd[0])<<24)|((#abcd[1])<<16)|((#abcd[2])<<8)|(#abcd[3]))
#define TAG3(abc)  TAG4(abc##0)

// memory wrappers

#if 0
#define CALLOC calloc
#define FREE free
#define MALLOC malloc
#define REALLOC realloc
#define STRDUP strdup
#else
#include "core/realloc.c"
#endif

#if MSC
#include <malloc.h>
#define ALLOCA(type,name,sz) type *name = (type*)_alloca(sz)
#else
#define ALLOCA(type,name,sz) type name[sz]
#endif

// builtin macros

#define builtin(x, ...) builtin_##x (__VA_ARGS__)

#if MSC
#define builtin_inline() __inline
#define builtin_thread() __declspec(thread)
#define builtin_format_string()
#define builtin_format_vargs(num_args)
#define builtin_puts(text) puts(text)
#define builtin_callstack(num_traces) STRDUP("")
#define builtin_inline() __inline
#else
#define builtin_inline() inline
#define builtin_thread() __thread
#define builtin_format_string()
#define builtin_format_vargs(num_args)
#define builtin_puts(text) puts(text)
#define builtin_callstack(num_traces) STRDUP("")
#define builtin_inline() inline
#endif

// perf profiling macro

#ifndef $
#define $ /* printf("%s:%d\n", __FUNCTION__,__LINE__); */
#endif

// units and constants

#define u_1e9 *1000000000ULL
#define u_1e6 *1000000ULL
#define u_1e3 *1000ULL

#define u_TiB *1099511627776ULL
#define u_GiB *1073741824ULL
#define u_MiB *1048576ULL
#define u_KiB *1024ULL

#define u_ss *1000000000LL
#define u_ms *1000000LL
#define u_us *1000LL
#define u_ns *1LL

#define u_Km *1000000LL
#define u_m  *1000LL
#define u_ft *304.8
#define u_dm *100LL
#define u_cm *10LL
#define u_mm *1LL

#define u_kg *1000LL
#define u_lb *453.59237
#define u_g  *1LL

#define u_degrees  *(3.14159f/180.f)
#define u_radians  *1.f //(180.f/3.14159f)

#define u_bytes *1ULL
#define u_bits  *0.125

// types

typedef unsigned usize;    // 32bit size_t
typedef int      isize;    // 32bit ssize_t

IF(MSC, typedef ptrdiff_t ssize_t); // 64bit ssize_t for MSC

// string helpers

#ifdef _MSC_VER
#define __thread __declspec(thread)
#else
#define __inline inline
#endif
#ifndef VSNPRINTF
#define VSNPRINTF vsnprintf
#endif
#include <stdarg.h>
#include <assert.h>
static __thread char vav_buf[2048];
static __thread int  vav_idx = 0;
static __inline char *vav( const char *fmt, va_list vl ) {
    int l = vsnprintf(0, 0, fmt, vl );
    assert(l >= 0);
    assert(l+1 <= 2048);
    char *dst = vav_buf + (vav_idx + l < 2048 ? vav_idx : 0);
    vav_idx += VSNPRINTF( dst, 2048, fmt, vl );
    return dst;
}
static __inline char *va( const char *fmt, ... ) {
    va_list vl;
    va_start(vl, fmt);
    char *dst = vav( fmt, vl );
    va_end(vl);
    return dst;
}

#endif /* AVA_VERSION */

// ----------------------------------------------------------------------------
// modules

#ifdef __cplusplus
extern "C" {
#endif

#include "core/detect.c"

#include "ds/ds.c"

#include "async/async.c"
#include "debug/debug.c"
#include "os/os.c"
#include "ecs/ecs.c"
#include "serial/json5.c"
#include "framework/framework.c"
#include "audio/audio.c"

#ifdef __cplusplus
}
#endif

