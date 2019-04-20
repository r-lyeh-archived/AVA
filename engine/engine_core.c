#ifdef __cplusplus
extern "C" {
#endif

#ifdef CORE_C
#pragma once
#define ASYNC_C
#define DEBUG_C
#define DETECT_C
#define MEMORY_C
#define OBJECT_C
#define NATIVE_C
#define STRING_C
#define STRUCT_C
#define TIME_C
#endif

#include "engine_config.c"

#include "object/object.c"
#include "detect/detect.c"
#include "memory/memory.c"
#include "struct/struct.c"
#include "async/async.c"
#include "debug/debug.c"
#include "native/native.c"
#include "time/time.c"
#include "string/string.c"

#include "math/math.c"

#ifdef __cplusplus
}
#endif
