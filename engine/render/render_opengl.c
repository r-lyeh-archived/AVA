#ifndef OPENGL_H
#define OPENGL_H

#ifdef OPENGL_C
#define GL3W_IMPLEMENTATION
#define GLAD_IMPLEMENTATION
#endif

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

#ifndef GLAPI
#define GLAPI API
#endif
#include "../3rd/glad.h"
//#include "../3rd/gl3w.h"

#include "../3rd/gl_portable/gl_portable.h"
#include <stdint.h>
typedef uint32_t GLuint;

// extra def
#ifndef GL_TEXTURE_MAX_ANISOTROPY_EXT
#define GL_TEXTURE_MAX_ANISOTROPY_EXT 0x84FE
#endif

// gpu timer
API int64_t gputime();

// error checking

#ifndef SHIPPING
    #define GL(func) do { \
        func; \
        for(GLenum err; GL_NO_ERROR != (err = glGetError()); ) { \
            const char *rc; \
            switch( err ) { \
                break; case GL_INVALID_ENUM:                   rc = "GL_INVALID_ENUM"; \
                break; case GL_INVALID_FRAMEBUFFER_OPERATION:  rc = "GL_INVALID_FRAMEBUFFER_OPERATION"; \
                break; case GL_INVALID_OPERATION:              rc = "GL_INVALID_OPERATION"; \
                break; case GL_INVALID_VALUE:                  rc = "GL_INVALID_VALUE"; \
                break; case GL_NO_ERROR:                       rc = "GL_NO_ERROR"; \
                break; case GL_OUT_OF_MEMORY:                  rc = "GL_OUT_OF_MEMORY"; \
                break; case GL_STACK_OVERFLOW:                 rc = "GL_STACK_OVERFLOW"; \
                break; case GL_STACK_UNDERFLOW:                rc = "GL_STACK_UNDERFLOW"; \
                break; default:                                rc = "UNKNOWN GL ERROR"; \
            } \
            LOGERROR(OPENGL, "!OpenGL ERROR %08x (%s) ; after executing: %s", err, rc, #func); \
        } \
    } while (0)
#else
    #define GL(func) func
#endif

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

int64_t gputime() {
    GLint64 t = 123456789;
    glGetInteger64v(GL_TIMESTAMP, &t);
    return (int64_t)t;
}

#endif
