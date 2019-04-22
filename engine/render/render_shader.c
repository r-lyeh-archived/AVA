#ifndef SHADER_H
#define SHADER_H
#include "render_opengl.c"

#define OpenGL20 "#version 110\n"
#define OpenGL21 "#version 120\n"
#define OpenGL30 "#version 130\n"
#define OpenGL31 "#version 140\n"
#define OpenGL32 "#version 150 core\n"
#define OpenGL33 "#version 330 core\n" // layout
#define OpenGL40 "#version 400 core\n"
#define OpenGL41 "#version 410 core\n"
#define OpenGL42 "#version 420 core\n"
#define OpenGL43 "#version 430 core\n"
#define OpenGL44 "#version 440 core\n"

#define GLSL(...) #__VA_ARGS__

#define NO_LAYOUT \
    "#define layout(n)\n"

#define HAS_LAYOUT \
    "#extension GL_ARB_explicit_attrib_location : enable\n" /*EXT?*/ \
/*    "#extension GL_ARB_separate_shader_objects : enable\n"*/
    /* "#define layout(n)\n" \ */

#define HAS_UNIFORM_LAYOUT \
    /*"#extension GL_ARB_explicit_uniform_location : require\n" //enable
    */

/*
    "uniform float width = 1.f;"
    "uniform float height = 1.f;"
    "uniform float time = 0.f;"
    * version numbers automatically added as the first line of the shader code
    * language specific define macros to add custom shader code in for a specific language.
    * attribute -> in (vertex shader)
    * varying -> out (vertex shader)
    * varying -> in (fragment shader)
    * texture2D -> texture
    * texture3D -> texture
    * gl_FragColor -> Generated output variable
    * GLSL 120 - #ifdef GL21
    * GLSL 330 - #ifdef GL33
*/

#define ALIASES \
    "#define GL_TEXCOORD  texCoord\n" \
    "#define GL_FRAGCOLOR fragColor\n" \
    "#define TEXCOORD  texCoord\n" \
    "#define FRAGCOLOR fragColor\n" \
    "#define TexCoord  texCoord\n" \
    "#define FragColor fragColor\n" \
    "#define texcoord  texCoord\n" \
    "#define fragcolor fragColor\n"

#define UNIFORMS \
    "uniform float time = 0;\n" \
    "uniform float width = 1;\n" \
    "uniform float height = 1;\n"

#define COMMON \
    "#ifdef GL_ES\n" \
    "# define LOWP lowp\n" \
    "# define MED mediump\n" \
    "# define HIGH highp\n" \
    "# ifdef GL_FRAGMENT_PRECISION_HIGH\n" \
    "   precision highp float;\n" \
    "# else\n" \
    "   precision mediump float;\n" \
    "# endif\n" \
    "#else\n" \
    "# define LOWP\n" \
    "# define MED\n" \
    "# define HIGH\n" \
    "#endif\n" \
    UNIFORMS \
    ALIASES

#define VS120 \
    "#version 120\n" \
    COMMON \
    NO_LAYOUT \
    "#define in attribute\n" \
    "#define IN attribute\n" \
    "#define out varying\n" \
    "#define OUT varying\n"

#define VS130 \
    "#version 130\n" \
    COMMON \
    HAS_LAYOUT /*gl3*/ \
    /*NO_LAYOUT: gl2 */ \
    "#define OUT out\n" \
    "#define attribute in\n" \
    "#define ATTRIBUTE in\n" \
    "#define varying out\n" \
    "#define VARYING out\n"

#define VS140 \
    "#version 140\n" \
    COMMON \
    HAS_LAYOUT /*gl3*/ \
    /*NO_LAYOUT: gl2 */ \
    "#define OUT out\n" \
    "#define attribute in\n" \
    "#define ATTRIBUTE in\n" \
    "#define varying out\n" \
    "#define VARYING out\n"

#define VS150 \
    "#version 150\n" \
    COMMON \
    HAS_LAYOUT \
    "#define OUT out\n" \
    "#define attribute in\n" \
    "#define ATTRIBUTE in\n" \
    "#define varying out\n" \
    "#define VARYING out\n"

#define VS330 \
    "#version 330\n" \
    COMMON \
    "#define OUT out\n" \
    "#define attribute in\n" \
    "#define ATTRIBUTE in\n" \
    "#define varying out\n" \
    "#define VARYING out\n"

#define FS120 \
    "#version 120\n" \
    COMMON \
    NO_LAYOUT \
    "#define in varying\n" \
    "#define IN varying\n" \
    "#define texCoord gl_TexCoord[0]\n" \
    "#define fragColor gl_FragColor\n" \
    "#define texture texture2D\n" \
    "#define sampler sampler2D\n" 

#define FS130 \
    "#version 130\n" \
    COMMON \
    NO_LAYOUT \
    "#define IN in\n" \
    "#define varying in\n" \
    "#define VARYING in\n" \
/*  "#define texCoord gl_TexCoord[0]\n" */ \
    "#define fragColor gl_FragColor\n" \
/*    "#define texture texture2D\n"*/ \
/*    "#define sampler sampler2D\n" */

#define FS140 \
    "#version 140\n" \
    COMMON \
    NO_LAYOUT \
    "#define IN in\n" \
    "#define varying in\n" \
    "#define VARYING in\n" \
/*  "#define texCoord gl_TexCoord[0]\n" */ \
    "#define fragColor gl_FragColor\n" \
/*    "#define texture texture2D\n"*/ \
/*    "#define sampler sampler2D\n" */

#define FS150 \
    "#version 150\n" \
    COMMON \
    NO_LAYOUT \
    "#define IN in\n" \
    "#define varying in\n" \
    "#define VARYING in\n" \
    "in vec2 texCoord;\n" \
    "out vec4 fragColor;\n"

#define FS330 \
    "#version 330\n" \
    COMMON \
    NO_LAYOUT \
    "#define IN in\n" \
    "#define varying in\n" \
    "#define VARYING in\n" \
    "in vec2 texCoord;\n" \
    "out vec4 fragColor;\n"

#endif

// ----------------------------------------------------------------------------

#ifdef SHADER_C
#pragma once

#endif
