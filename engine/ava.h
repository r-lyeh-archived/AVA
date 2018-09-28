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

// detect platform

#include "core/detect/detect.c"
/*
#include "detect/defines.c"
#include "detect/includes.c"
#include "detect/linkage.c"
#include "detect/api.c"
#include "detect/detect.c"
#include "detect/builtin.c"
#include "detect/types.c"
*/

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
    vav_idx += VSNPRINTF( dst, 2048, fmt, vl ) + 1;
    return dst;
}
static __inline char *va( const char *fmt, ... ) {
    va_list vl;
    va_start(vl, fmt);
    char *dst = vav( fmt, vl );
    va_end(vl);
    return dst;
}

// ----------------------------------------------------------------------------
// modules

#include "core/core.c"

#include "framework/framework.c"
#include "framework/audio/audio.c"
#include "framework/asset/asset.c"

#include "media/Media.c"

#include "render/render.c"
#include "scene/scene.c"
#include "world/world.c"

#ifdef __cplusplus
}
#endif

#endif /* AVA_VERSION */
