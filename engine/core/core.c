#ifdef CORE_C
#pragma once
#define ASYNC_C
#define DEBUG_C
#define DETECT_C
#define FILESYS_C
#define MATH_C
#define NATIVE_C
#define OBJECT_C
#define SERIAL_C
#define DS_C
#define TIME_C
#endif

#include "detect/detect.c"

// memory wrappers
#include "object/object.c"

#include "ds/ds.c"

#include "async/async.c"
#include "debug/debug.c"
#include "filesys/filesys.c"
#include "math/math.c"
#include "native/native.c"
#include "serial/serial.c"
#include "time/time.c"
