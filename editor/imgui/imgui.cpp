/*
#include "engine.h"
*/
/*
#include <assert.h>
#define API
#define ASSERT assert
#define EXPORT __declspec(dllexport)
#define OSCRECV_C
#include "network/network_oscrecv.c"
#define SHAPE_C
#include "render/render_shape.c"
#include <stdint.h>
#define GL(...) __VA_ARGS__
*/

#include <stdint.h>
#define API
#include "engine_minimal.c"
#define OSCRECV_C
#include "network/network_oscrecv.c"
#define SHAPE_C
#include "render/rendernode_shape.c"
#define GL(...) __VA_ARGS__

#pragma comment(lib, "opengl32.lib")
#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif
#include "imgui.h"
#include "misc/freetype/imgui_freetype.h"
#include "imgui_internal.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
using namespace ImGui;
// About OpenGL function loaders: modern OpenGL doesn't have a standard header file and requires individual function pointers to be loaded manually. 
// Helper libraries are often used for this purpose! Here we are supporting a few common ones: gl3w, glew, glad.
// You may use another loader/header of your choice (glext, glLoadGen, etc.), or chose to manually implement your own.
#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
#include <GL/gl3w.h>    // Initialize with gl3wInit()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
#include <GL/glew.h>    // Initialize with glewInit()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
#include <glad/glad.h>  // Initialize with gladLoadGL()
#else
#include IMGUI_IMPL_OPENGL_LOADER_CUSTOM
#endif
#ifdef max
#undef max
#endif
#ifdef min
#undef min
#endif
#include <algorithm>
#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif
#include <math.h>


// incl render/set_render
#include "imgui/imgui_videorec.cpp"

#include "imgui/imgui_config.cpp"
#include "imgui/imgui_fonts.cpp"
#include "imgui/imgui_palette.cpp"
#include "imgui/imgui_theme.cpp"
#include "imgui/imgui_dsl.cpp"
// --
#include "imgui/imgui_icons.cpp"
#include "imgui/imgui_pangram.cpp"
#include "imgui/imgui_utils.cpp"    // for menubar below
#include "imgui/imgui_menubar.cpp"

#include "imgui/imgui_widgets.cpp" // must be first. widgets, all of them.
#include "imgui/imgui_property.cpp" // must be last

