#ifndef FRAMEWORK_H
#define FRAMEWORK_H

#ifndef UNICODE
#define UNICODE // for glfw3.c (win32)
#endif

#define GLAD_IMPLEMENTATION

#include "engine.h"

#endif

// 
// #include "opengl.c"
// #include "3rd/GLFW/glfw3.h"

#ifdef FRAMEWORK_C
#pragma once
#define APP_C
#define CAMERA_C
#define GLFW3_C
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

#include "glfw3.c"

#include "app.c"
#include "camera.c"
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
