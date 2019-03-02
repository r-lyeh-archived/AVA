#ifdef CONFIG_H
#include CONFIG_H
#endif

// @todo -> rename as MACROS instead

#ifdef DETECT_C
#pragma once
#define API_C
#define AUTORUN_C
#define BITS_C
#define CALLSTACK_C
#define INCLUDES_C
#define INLINE_C
#define LINKAGE_C
#define MEMORY_C
#define PLATFORM_C
#define RESTRICT_C
#define SHORTFILE_C
#define TEST_C
#define THREADLOCAL_C
#define TYPES_C
#endif

#include "detect_api.c"
#include "detect_autorun.c"
#include "detect_bits.c"
#include "detect_callstack.c"
#include "detect_includes.c"
#include "detect_inline.c"
#include "detect_linkage.c"
#include "detect_memory.c"
#include "detect_platform.c"
#include "detect_restrict.c"
#include "detect_shortfile.c"
#include "detect_test.c"
#include "detect_threadlocal.c"
#include "detect_types.c"
