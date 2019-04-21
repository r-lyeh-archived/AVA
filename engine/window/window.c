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
#define UI_C
#define ICON_C
#define TRAY_C
#endif

#include "../render/render.c" // opengl, pass

#include "window_ui.c"
#include "window_icon.c"
#include "window_tray.c"
#include "window_app.c"
#include "window_module.c"
#include "window_getopt.c"
