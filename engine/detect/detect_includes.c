#ifndef INCLUDES_C
#define INCLUDES_C

// System defines here

#ifndef _ALLOW_KEYWORD_MACROS
#define _ALLOW_KEYWORD_MACROS // msc
#endif
#ifndef _BSD_SOURCE
#define _BSD_SOURCE
#endif
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS // msc
#endif
#ifndef _DEFAULT_SOURCE
#define _DEFAULT_SOURCE
#endif
#ifndef _FILE_OFFSET_BITS
#define _FILE_OFFSET_BITS 64
#endif
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#ifndef _LARGE_FILE
#define _LARGE_FILE 1
#endif
#ifndef _LARGEFILE_SOURCE
#define _LARGEFILE_SOURCE 1
#endif
#ifndef _LARGEFILE64_SOURCE
#define _LARGEFILE64_SOURCE
#endif
#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 199309L
#endif
#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES // msc
#endif
#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE 700
#endif
#ifndef NOMINMAX
#define NOMINMAX // win
#endif
#ifndef UNICODE
#define UNICODE // win
#endif
#ifndef _UNICODE
#define _UNICODE // win
#endif
#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN // msc
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN 1 // win
#endif

// OS includes here

#if _WIN32
#include <winsock2.h>
#include <shlobj.h>
#include <io.h>
#else
#include <unistd.h>
#endif

// C includes here

#include <assert.h>
#include <math.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#endif
