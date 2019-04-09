#ifndef WINDOW_H
#define WINDOW_H
#   ifndef UNICODE
#   define UNICODE // for glfw3.c (win32)
#   endif
#   define GLAD_IMPLEMENTATION
#   include "../engine.h"
#endif

#ifdef WINDOW_C
#pragma once
#define APP_C
#define MODULE_C
#define GETOPT_C
#endif

#include "../render/render.c" // opengl, pass

#include "window_app.c"
#include "window_module.c"
#include "window_getopt.c"
//#include "network.c"
//#include "palette.c"
//#include "memory.c"
//#include "string32.c"
//#include "stringv.c"

#ifdef APP_DEMO
int main() {
    window_create(0.75f, 0);

    while( window_update() ) {
        window_swap(NULL);
    }
}
#endif
