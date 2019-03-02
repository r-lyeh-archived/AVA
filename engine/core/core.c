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
#define STRING_C
#define STRUCT_C
#define TIME_C
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
