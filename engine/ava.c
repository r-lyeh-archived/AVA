//                     /\\ \// /\\
//                  small game engine
//
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.
//
// People who worked on this file: @r-lyeh,

#  if !defined(API) && defined(__cplusplus)
#   define API extern "C" __declspec(dllexport)
#elif !defined(API)
#   define API            __declspec(dllexport)
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define ASYNC_C
#define DEBUG_C
#define ECS_C
#define VALLOC_C
#define JSON5_C

#include "async/async.c"
#include "debug/debug.c"
#include "ecs/ecs.c"
#include "serial/json5.c"

API
int init() {
    return 0;
}

#ifdef __cplusplus
}
#endif
