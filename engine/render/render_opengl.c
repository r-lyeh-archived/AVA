#ifndef OPENGL_H
#define OPENGL_H

// { mini KHR

// Generic fallback
#include <stdint.h>
typedef int8_t                  khronos_int8_t;
typedef uint8_t                 khronos_uint8_t;
typedef int16_t                 khronos_int16_t;
typedef uint16_t                khronos_uint16_t;
typedef int32_t                 khronos_int32_t;
typedef uint32_t                khronos_uint32_t;
typedef int64_t                 khronos_int64_t;
typedef uint64_t                khronos_uint64_t;
#define KHRONOS_SUPPORT_INT64   1
#define KHRONOS_SUPPORT_FLOAT   1

#ifdef _WIN64
typedef signed   long long int khronos_intptr_t;
typedef unsigned long long int khronos_uintptr_t;
typedef signed   long long int khronos_ssize_t;
typedef unsigned long long int khronos_usize_t;
#else
typedef signed   long  int     khronos_intptr_t;
typedef unsigned long  int     khronos_uintptr_t;
typedef signed   long  int     khronos_ssize_t;
typedef unsigned long  int     khronos_usize_t;
#endif

typedef float khronos_float_t;

// } mini KHR

#define GLAPI API
#include "../3rd/glad.h"
//#include "../3rd/gl3w.h"

#include "../3rd/gl_portable/gl_portable.h"
#include <stdint.h>
typedef uint32_t GLuint;

#endif


#ifdef OPENGL_C /* GLAD_IMPLEMENTATION */
#pragma once

#ifdef _MSC_VER
#pragma comment(lib, "opengl32.lib")
#endif

#include "../3rd/glad.c"
//#define GL3W_IMPLEMENTATION
//#include "../3rd/gl3w.h"

#include "../3rd/gl_portable/gl_portable.c"

#endif
