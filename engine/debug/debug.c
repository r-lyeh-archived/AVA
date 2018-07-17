#pragma once
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

#ifndef MALLOC
#define MALLOC malloc
#define FREE free
#define REALLOC realloc
#endif

#ifndef HEAP
#define HEAP
#endif

#if defined(_MSC_VER) && !defined(__thread)
#define __thread __declspec(thread)
#endif

#ifdef DEBUG_C
#pragma once
#define ASSERT_C 1
#define BENCHMARK_C 1
#define BREAKPOINT_C 1
#define BUILD_C 1
#define CALLSTACK_C 1
#define CONSOLE_C 1
#define CRASH_C 1
#define DEBUGGER_C 1
#define DIALOG_C 1
#define HEXDUMP_C 1
#define LOGGER_C 1
#define PANIC_C 1
#define TEST_C 1
#endif

#include "debug_assert.c"
#include "debug_benchmark.c"
#include "debug_breakpoint.c"
#include "debug_build.c"
#include "debug_callstack.c"
#include "debug_console.c"
#include "debug_crash.c"
#include "debug_debugger.c"
#include "debug_dialog.c"
#include "debug_hexdump.c"
#include "debug_logger.c"
#include "debug_panic.c"
#include "debug_test.c"
