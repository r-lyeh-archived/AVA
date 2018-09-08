#ifndef FRAMEWORK_H
#define FRAMEWORK_H

#ifndef UNICODE
#define UNICODE // for glfw3.c (win32)
#endif

#define GLAD_IMPLEMENTATION

#if 0
#include "ava.h"
#else
//#include "defines.c"
#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif

#ifndef SYS_WIN
	#define SYS_WIN 1
	#define SYS_MSC 1
	#define MSC 1
	#define WIN 1
	#define IF(x, true_, ...)  IF_(x, true_, __VA_ARGS__)
	#define IFN(x, true_, ...) IF_(x, __VA_ARGS__, true_)
	#define IF_(x, true_, ...) IF_##x(true_, __VA_ARGS__)
	#define IF_0(true_,...)    __VA_ARGS__
	#define IF_1(true_,...)    true_
	#endif
#endif

//#include "includes.c"
#include <math.h>
#include <stdbool.h>
#include <stdint.h>

#endif

// 
// #include "opengl.c"
// #include "3rd/GLFW/glfw3.h"

#ifdef FRAMEWORK_C
#pragma once
#define APP_C
#define CAMERA_C
#define COLOR_C
#define FILESYS_C
#define GLFW3_C
#define IMAGE_C
#define INPUT_C
//#define LINEAR_C
//#define MATH_C
#define NETWORK_C
//#define NOISE_C
#define OPENGL_C
#define RENDER_C
//#define TRANSFORM_C
#define WINDOW_C
#endif

#include "opengl.c"

#ifdef _WIN32
//#include <windows.h>
//#include <mmsystem.h>
#endif

#include "glfw3.c"

#include "app.c"
#include "camera.c"
#include "color.c"
#include "filesys_.c"
#include "image.c"
#include "input.c"
//#include "linear.c"
//#include "math.c"
//#include "network.c"
//#include "noise.c"
#include "render.c"
//#include "transform.c"
#include "window.c"

// #include "palette.c"
//#include "memory.c"
//#include "string32.c"
//#include "stringv.c"

#ifdef FRAMEWORK_DEMO
int main() {
    IF(WIN, puts("win"), puts("not win"));
    IF(MSC, puts("msc"), puts("not msc"));

    window_create(0.85f, 0);

    int w, h;
    void *pixels = 0;
    while( window_update(&w, &h) ) {
        window_swap(&pixels);
    }
}
#endif
