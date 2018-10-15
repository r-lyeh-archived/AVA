#include "engine.h"

#ifdef ENGINE_C
#pragma once
#define CORE_C
#define FRAMEWORK_C
#define AUDIO_C
#define ASSET_C
#define MEDIA_C
#define RENDER_C
#define SCENE_C
#define WORLD_C
#endif

#ifdef CORE_C
#pragma once
#define ASYNC_C
#define DEBUG_C
#define DETECT_C
#define FILESYS_C
#define MATH_C
#define MEMORY_C
#define NATIVE_C
#define OBJECT_C
#define SERIAL_C
#define STRUCT_C
#define TIME_C
#define STRING_C
#endif
#include "detect/detect.c"
#include "memory/memory.c"
#include "object/object.c"
#include "struct/struct.c"
#include "async/async.c"
#include "debug/debug.c"
#include "filesys/filesys.c"
#include "math/math.c"
#include "native/native.c"
#include "serial/serial.c"
#include "time/time.c"
#include "string/string.c"


#include "framework/framework.c"

#include "media/media.c"

#include "framework/audio/audio.c"

#include "render/render.c"
#include "scene/scene.c"
#include "world/world.c"

API int init();

int init() {
    vfs_import("data/**");
    vfs_import("game/**");
    vfs_import("games/**");
    vfs_import("assets/**");
    void (*ptr)();
    for( int i = 0; i < 128; ++i ) {
        if( 0 != (ptr = dllquick( "game.dll", "main" ) ) ) {
            ptr();
        }
        if( 0 != (ptr = dllquick( va("game%d.dll", i), "main" ) ) ) {
            ptr();
        }
        if( 0 != (ptr = dllquick( va("game%02d.dll", i), "main" ) ) ) {
            ptr();
        }
        if( 0 != (ptr = dllquick( va("game%03d.dll", i), "main" ) ) ) {
            ptr();
        }
    }
    return 0;
}

// #ifdef _MSC_VER
// #define __thread __declspec(thread)
// #else
// #define __inline inline
// #endif
#include <stdarg.h>
#include <assert.h>
static THREAD_LOCAL char vl_buf[2048];
static THREAD_LOCAL int  vl_idx = 0;
char *vl( const char *fmt, va_list v ) {
    int l = VSNPRINTF(0, 0, fmt, v );
    assert(l >= 0);
    assert(l+1 <= 2048);
    char *dst = vl_buf + (vl_idx + l < 2048 ? vl_idx : 0);
    vl_idx += VSNPRINTF( dst, 2048, fmt, v ) + 1;
    return dst;
}
char *va( const char *fmt, ... ) {
    va_list v;
    va_start(v, fmt);
    char *dst = vl( fmt, v );
    va_end(v);
    return dst;
}
