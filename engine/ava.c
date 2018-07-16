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

#define ASYNC_C 1
#define DEBUG_C 1
#define ECS_C 1
#define VALLOC_C 1

#include "async/async.c"
#include "debug/debug.c"
#include "ecs/ecs.c"

API
int init() {
    return 0;
}
