#ifndef APP_H
#define APP_H
#   ifndef UNICODE
#   define UNICODE // for glfw3.c (win32)
#   endif
#   define GLAD_IMPLEMENTATION
#   include "../engine.h"
#endif

#ifdef APP_C
#pragma once
#define MODULE_C
#define WINDOW_C
#endif

#include "../render/render.c" // opengl, pass

#include "module.c"
#include "window.c"

//#include "network.c"
//#include "palette.c"
//#include "memory.c"
//#include "string32.c"
//#include "stringv.c"

#ifdef APP_DEMO
int main() {
    window_create(0.85f, 0);

    int w, h;
    void *pixels = 0;
    while( window_update(&w, &h) ) {
        window_swap(&pixels);
    }
}
#endif
