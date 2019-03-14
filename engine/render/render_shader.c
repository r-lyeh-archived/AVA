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

// shader
API GLuint shader( const char *vert, const char *frag );
API void shader_bind( unsigned shader );

typedef GLuint Shader;
typedef GLuint Buffer;
typedef GLuint VAO;
typedef GLuint Attribute;

API Attribute get_attribute_location( Shader p, const char *name );

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
    "#define out varying\n"

#define VS130 \
    "#version 130\n" \
    COMMON \
    HAS_LAYOUT /*gl3*/ \
    /*NO_LAYOUT: gl2 */ \
    "#define attribute in\n" \
    "#define varying out\n"

#define VS140 \
    "#version 140\n" \
    COMMON \
    HAS_LAYOUT /*gl3*/ \
    /*NO_LAYOUT: gl2 */ \
    "#define attribute in\n" \
    "#define varying out\n"

#define VS150 \
    "#version 150\n" \
    COMMON \
    HAS_LAYOUT \
    "#define attribute in\n" \
    "#define varying out\n"

#define VS330 \
    "#version 330\n" \
    COMMON \
    "#define attribute in\n" \
    "#define varying out\n"

#define FS120 \
    "#version 120\n" \
    COMMON \
    NO_LAYOUT \
    "#define in varying\n" \
    "#define texCoord gl_TexCoord[0]\n" \
    "#define fragColor gl_FragColor\n" \
    "#define texture texture2D\n" \
    "#define sampler sampler2D\n" 

#define FS130 \
    "#version 130\n" \
    COMMON \
    NO_LAYOUT \
    "#define varying in\n" \
/*  "#define texCoord gl_TexCoord[0]\n" */ \
    "#define fragColor gl_FragColor\n" \
/*    "#define texture texture2D\n"*/ \
/*    "#define sampler sampler2D\n" */

#define FS140 \
    "#version 140\n" \
    COMMON \
    NO_LAYOUT \
    "#define varying in\n" \
/*  "#define texCoord gl_TexCoord[0]\n" */ \
    "#define fragColor gl_FragColor\n" \
/*    "#define texture texture2D\n"*/ \
/*    "#define sampler sampler2D\n" */

#define FS150 \
    "#version 150\n" \
    COMMON \
    NO_LAYOUT \
    "#define varying in\n" \
    "in vec2 texCoord;\n" \
    "out vec4 fragColor;\n"

#define FS330 \
    "#version 330\n" \
    COMMON \
    NO_LAYOUT \
    "#define varying in\n" \
    "in vec2 texCoord;\n" \
    "out vec4 fragColor;\n"

#endif

// ----------------------------------------------------------------------------

#ifdef SHADER_C
#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "render_opengl.c"
#include "../engine.h" // realloc

static
void dump_shader_log( const char *source ) {
    // dump source with line numbers
    int line = 1;
    const char *from = source, *end = strchr(from, '\n');
    while( end ) {
        fprintf(stderr, "%04d:%.*s\n", line++, (int)(end - from), from);
        end = strchr( from = end + 1, '\n');
    }
    fprintf(stderr, "%04d:%s\n", line++, from);
}

static
GLuint shader_generate( GLenum type, const char *source ) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, (const char **)&source, NULL);
    glCompileShader(shader);

    GLint statuslen = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &statuslen);
    if( statuslen == GL_FALSE ) {
        //GLchar *logbuf = 0; // pros: clean + standard, cons: slow if compiling thousand of shaders?
        //static GLchar *logbuf = 0; // cons: faster, cons: no MT + 1 memleak.
        char logbuf[2048] = { 0 }; // enough?

        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &statuslen);
        //logbuf = (GLchar*)REALLOC(logbuf, statuslen);
        glGetShaderInfoLog(shader, statuslen, NULL, logbuf);

        // dump log with line numbers
        dump_shader_log( source );
        fprintf(stderr, "ERROR: %s shader:\n%s\n", type == GL_VERTEX_SHADER ? "Vertex" : "Fragment", logbuf);

        //FREE(buf);
        shader = 0;
    }

    return shader;
}

GLuint shader(const char *vs, const char *fs) {
    GLuint vert = shader_generate(GL_VERTEX_SHADER, vs);
    GLuint frag = shader_generate(GL_FRAGMENT_SHADER, fs);
  //GLuint geom = shader_generate(GL_GEOMETRY_SHADER, gs);
    GLuint program = 0;

    if( vert && frag ) {
        program = glCreateProgram();

        glAttachShader(program, vert);
        glAttachShader(program, frag);
        // glAttachShader(program, geom);

        glLinkProgram(program);

        GLint status = GL_FALSE, length;
        glGetProgramiv(program, GL_LINK_STATUS, &status);
        if (status == GL_FALSE) {
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
            GLchar *buf = (GLchar*)MALLOC(length);
            glGetProgramInfoLog(program, length, NULL, buf);
            fprintf(stderr, "Error: Shader/program link: %s\n", buf);
            FREE(buf);
            return 0;
        }

        // glDetachShader(program, vert);
        // glDetachShader(program, frag);
        // glDetachShader(program, geom);

        glDeleteShader(vert);
        glDeleteShader(frag);
        // glDeleteShader(geom);
    }

    return program;
}

void shader_bind(unsigned shader) {
    glUseProgram(shader);
}

/*
GLuint shader_create( const char *vert, const char *frag ) {
    GLuint vertex_shader, fragment_shader, program;
    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL);
    glCompileShader(vertex_shader);
    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_text, NULL);
    glCompileShader(fragment_shader);
    program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);
    return program;
}
*/

Attribute get_attribute_location( Shader p, const char *name ) {
    glUseProgram(p);
    Attribute rc = glGetAttribLocation(p, name);
    glUseProgram(0);
    return rc;
}

#endif
