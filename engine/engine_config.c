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

#define TYPE(abcd) ((0[#abcd"0000"]<<0)|(1[#abcd"0000"]<<8)|(2[#abcd"0000"]<<16)|(3[#abcd"0000"]<<24))
#define TAG4(abcN) TYPE(abcN)

// code perf/profiling

#ifndef $
#define $ /* printf("%s:%d\n", __FUNCTION__,__LINE__); */
#endif

#endif
