//                     /\\ \// /\\
//                  small game engine
//
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.
//
// People who worked on this file: @r-lyeh,

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
#endif

#include "core/core.c"

#include "async/async.c"
#include "debug/debug.c"
#include "os/os.c"
#include "ecs/ecs.c"
#include "serial/json5.c"
#include "framework/framework.c"

API int init();

int init() {
    void (*ptr)();
    if( 0 != (ptr = dllquick( "game01.dll", "main" ) ) ) {
        ptr();
    }
    if( 0 != (ptr = dllquick( "game02.dll", "main" ) ) ) {
        ptr();
    }
    return 0;
}
