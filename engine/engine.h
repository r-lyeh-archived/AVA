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

#ifdef __cplusplus
extern "C" {
#endif

#ifdef   CONFIG_H
#include CONFIG_H
#endif

// detect platform

#include "core/detect/detect.c"

// code annotations

#define HEAP           // heap pointer. must free() after use
#define TEMP           // temporary stack pointer. do not free() after use
#define GC             // garbage collected pointer. do not free() after use
#define FAT            // fat pointer (internal data at &[-1])

// code tags

#define TAG4(abcN) ((0[#abcN"00"]<<0)|(1[#abcN"00"]<<8)|(2[#abcN"00"]<<16)|(3[#abcN"00"]<<24))

// code perf/profiling

#ifndef $
#define $ /* printf("%s:%d\n", __FUNCTION__,__LINE__); */
#endif

// string helpers

#include <stdarg.h>
#ifndef VSNPRINTF
#define VSNPRINTF vsnprintf
#endif
API char *vl( const char *fmt, va_list vl );
API char *va( const char *fmt, ... );

// ----------------------------------------------------------------------------
// modules

#include "core/core.c"
#include "framework.c"
#include "server.c"

#ifdef __cplusplus
}
#endif

#endif /* AVA_VERSION */

// ----------------------------------------------------------------------------
// #define warn(...)          ( fprintf(stderr, __VA_ARGS__), 1 )
// #define die(...)           ( warn(__VA_ARGS__), exit(-__LINE__), 1 ) // use: test || die
