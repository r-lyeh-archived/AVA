#ifndef GLFW3_H
#define GLFW3_H

//#include "opengl.c"
#define GLFW_INCLUDE_NONE
#include "3rd/GLFW/glfw3.h"
#include "3rd/gleq.h"

#endif

#ifdef GLFW3_C
#pragma once

#define GLFW_INCLUDE_NONE
#include "3rd/GLFW/glfw3.h"
#undef GLFW_INCLUDE_NONE

#ifdef _MSC_VER
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "user32.lib")
#endif

//#define _GLFW_BUILD_DLL 1

/*
The window creation API is used to create windows, handle input, monitors, gamma ramps and clipboard. The options are:

_GLFW_COCOA to use the Cocoa frameworks
_GLFW_WIN32 to use the Win32 API
_GLFW_X11 to use the X Window System
_GLFW_WAYLAND to use the Wayland API (experimental and incomplete)
_GLFW_MIR to use the Mir API (experimental and incomplete)

The context creation API is used to enumerate pixel formats / framebuffer configurations and to create contexts. The options are:

_GLFW_NSGL to use the Cocoa OpenGL framework
_GLFW_WGL to use the Win32 WGL API
_GLFW_GLX to use the X11 GLX API
_GLFW_EGL to use the EGL API

Wayland and Mir both require the EGL backend.

The client library is the one providing the OpenGL or OpenGL ES API, which is used by GLFW to probe the created context. This is not the same thing as the client API, as many desktop OpenGL client libraries now expose the OpenGL ES API through extensions. The options are:

_GLFW_USE_OPENGL for the desktop OpenGL (opengl32.dll, libGL.so or OpenGL.framework)
_GLFW_USE_GLESV1 for OpenGL ES 1.x (experimental)
_GLFW_USE_GLESV2 for OpenGL ES 2.x (experimental)

Note that _GLFW_USE_GLESV1 and _GLFW_USE_GLESV2 may only be used with EGL, as the other context creation APIs do not interface with OpenGL ES client libraries.

If you are building GLFW as a shared library / dynamic library / DLL then you must also define _GLFW_BUILD_DLL. Otherwise, you may not define it.

If you are using the X11 window creation API, support for the following X11 extensions can be enabled:

_GLFW_HAS_XINPUT to use XInput2 for high-resolution cursor motion (recommended)
_GLFW_HAS_XF86VM to use Xxf86vm as a fallback when RandR gamma is broken (recommended)

If you are using the Cocoa window creation API, the following options are available:

_GLFW_USE_CHDIR to chdir to the Resources subdirectory of the application bundle during glfwInit (recommended)
_GLFW_USE_MENUBAR to create and populate the menu bar when the first window is created (recommended)
_GLFW_USE_RETINA to have windows use the full resolution of Retina displays (recommended)

*/

// null_init.c
// null_joystick.c
// null_monitor.c
// null_window.c

#ifdef _WIN32
#   define _GLFW_WIN32 1
#// define _GLFW_USE_OPENGL 1
#// define _GLFW_WGL 1
#// define _GLFW_USE_HYBRID_HPG 1
#   ifndef UNICODE
#   define UNICODE
#   endif
#   ifdef APIENTRY
#// undef APIENTRY
#   endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN 1
#endif
#ifndef NOMINMAX
#define NOMINMAX 1
#endif
#include <winsock2.h>
//#include <windows.h>
#include <mmsystem.h>
#   pragma comment(lib, "shell32.lib")
#   pragma comment(lib, "gdi32.lib")
#   pragma comment(lib, "user32.lib")
#   pragma comment(lib, "shell32.lib")
#   include "3rd/GLFW/win32_init.c"
#   include "3rd/GLFW/win32_joystick.c"
#   include "3rd/GLFW/win32_monitor.c"
#   include "3rd/GLFW/win32_thread.c"
#   include "3rd/GLFW/win32_time.c"
#   include "3rd/GLFW/win32_window.c"
#   include "3rd/GLFW/wgl_context.c"
#elif defined(__DARWIN__)
#   define _GLFW_COCOA 1
#// define _GLFW_USE_MENUBAR 1
#// define _GLFW_USE_CHDIR 1
#// define _GLFW_USE_RETINA 1
#   include "3rd/GLFW/cocoa_time.c"
#else
#   if 1
    // x11
#   define _GLFW_X11 1
#// define _GLFW_HAS_XF86VM 1
#   include "3rd/GLFW/x11_init.c"
#   include "3rd/GLFW/x11_monitor.c"
#   include "3rd/GLFW/x11_window.c"
#   include "3rd/GLFW/xkb_unicode.c"
#   include "3rd/GLFW/glx_context.c"
#   elif 0
    // wayland
#   define _GLFW_WAYLAND 1
#   include "3rd/GLFW/wl_init.c"
#   include "3rd/GLFW/wl_monitor.c"
#   include "3rd/GLFW/wl_window.c"
#   include "3rd/GLFW/glx_context.c"
#   elif 0
    // mir
#   define _GLFW_MIR 1
#   include "3rd/GLFW/mir_init.c"
#   include "3rd/GLFW/mir_monitor.c"
#   include "3rd/GLFW/mir_window.c"
#   include "3rd/GLFW/glx_context.c"
#   endif
#endif

#ifndef _WIN32
#   include "3rd/GLFW/posix_time.c"
#   include "3rd/GLFW/posix_thread.c"
#   ifdef __linux__
#       include "3rd/GLFW/linux_joystick.c"
#   endif
#endif

#include "3rd/GLFW/context.c"
#include "3rd/GLFW/egl_context.c"
#include "3rd/GLFW/init.c"
#include "3rd/GLFW/input.c"
#include "3rd/GLFW/monitor.c"
#include "3rd/GLFW/osmesa_context.c"
#include "3rd/GLFW/vulkan.c"
#include "3rd/GLFW/window.c"

#define GLEQ_IMPLEMENTATION
#include "3rd/gleq.h"

#endif
