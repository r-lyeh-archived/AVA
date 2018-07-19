#include "ava.h"

#ifdef AVA_C
#pragma once
#define CORE_C
#define ASYNC_C
#define DEBUG_C
#define ECS_C
#define OS_C
#define JSON5_C
#define FRAMEWORK_C
#define AUDIO_C
#endif

#include "core/core.c"

#include "async/async.c"
#include "debug/debug.c"
#include "os/os.c"
#include "ecs/ecs.c"
#include "serial/json5.c"
#include "framework/framework.c"
#include "audio/audio.c"

API int init();

int init() {
    void (*ptr)();
    for( int i = 0; i < 128; ++i ) {
        if( 0 != (ptr = dllquick( "game.dll", "main" ) ) ) {
            ptr();
        }
        if( 0 != (ptr = dllquick( format("game%d.dll", i), "main" ) ) ) {
            ptr();
        }
        if( 0 != (ptr = dllquick( format("game%02d.dll", i), "main" ) ) ) {
            ptr();
        }
        if( 0 != (ptr = dllquick( format("game%03d.dll", i), "main" ) ) ) {
            ptr();
        }
    }
    return 0;
}
