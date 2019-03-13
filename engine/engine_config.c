#ifndef ENGINE_CONFIG_H
#define ENGINE_CONFIG_H

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

#endif
