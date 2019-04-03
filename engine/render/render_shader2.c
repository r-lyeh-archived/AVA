#ifndef SHADER2_H
#define SHADER2_H

// shader

API unsigned shader( const char *vert, const char *frag );
API unsigned shader2(const char *vp, const char *fp, const char *csv_attributes);
API void shader_bind( unsigned shader );

API void shader_bind_int1(unsigned program, const char *uniform, int value);
API void shader_bind_texture(unsigned program, unsigned texture, unsigned unit);
API void shader_push_strip(unsigned program, unsigned vertex_vao, unsigned first, unsigned count);

API void shader_bind_int(unsigned program, const char *uniform, int value);
API void shader_bind_float(unsigned program, const char *uniform, float f);
API void shader_bind_vec2(unsigned program, const char *uniform, vec2);
API void shader_bind_vec3(unsigned program, const char *uniform, vec3);
API void shader_bind_vec4(unsigned program, const char *uniform, vec4);
API void shader_bind_mat44(unsigned program, const char *uniform, mat44);

API const char *shader_default_diffuse_fs();
API const char *shader_default_fullscreen_vs();

#endif

// ----------------------------------------------------------------------------

#ifdef SHADER2_C
#pragma once
#include "render_opengl.c"
#include <stdlib.h>

int UNIFORM_LOC(unsigned program,const char *name) {
    int ret =
    (name[0] >= '0' && name[0] <= '9' ? atoi(name) : glGetUniformLocation(program,name));
    if( ret < 0 ) fprintf(stderr, "cannot find uniform '%s' in shader program %d\n", name, (int)program );
    return ret;
}

void shader_bind_int(unsigned program, const char *name, int i)        { glUniform1i(UNIFORM_LOC(program,name), i); }
void shader_bind_float(unsigned program, const char *name, float f)    { glUniform1f(UNIFORM_LOC(program,name), f); }
void shader_bind_vec2(unsigned program, const char *name, vec2 v)  { glUniform2fv(UNIFORM_LOC(program,name), 1, &v.x); }
void shader_bind_vec3(unsigned program, const char *name, vec3 v)  { glUniform3fv(UNIFORM_LOC(program,name), 1, &v.x); }
void shader_bind_vec4(unsigned program, const char *name, vec4 v)  { glUniform4fv(UNIFORM_LOC(program,name), 1, &v.x); }
void shader_bind_mat44(unsigned program, const char *name, mat44 m) { glUniformMatrix4fv(UNIFORM_LOC(program,name), 1, GL_FALSE/*GL_TRUE*/, m); }

void shader_bind_int1(GLuint program, const char *uniform, int value) {
    glUniform1i(UNIFORM_LOC(program, uniform), value);
}

void shader_bind_texture(GLuint program, GLuint texture, GLuint unit) {
    glUseProgram(program);
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, texture);
}

void shader_push_strip(GLuint program, GLuint vertex_vao, GLuint first, GLuint count) {
    GLenum primitive = GL_TRIANGLE_STRIP;
    glUseProgram(program);
    glBindVertexArray(vertex_vao);
    glDrawArrays(primitive, first, count);
}

const char *shader_default_fullscreen_vs() {
    return
    VS130 // "#version 330 core\n"
    "out vec2 uv;\n"
    "void main() {\n"
    "   float x = gl_VertexID / 2;\n"
    "   float y = gl_VertexID % 2;\n"
    "   uv = vec2(x, y);\n"
    "   gl_Position = vec4(2.0*uv - 1.0, 0.0, 1.0);\n"
    "}\n";
}

const char *shader_default_diffuse_fs() {
    return
    FS130 // "#version 330 core\n"
    "in vec2 uv;\n"
    "uniform sampler2D texture_sampler;\n"
    "out vec4 color;\n"
    "void main() {\n"
    "   color = vec4(texture(texture_sampler, uv).rgb, 1.0);\n"
    "}\n";
}

#include <stdint.h>
#include <stdio.h>

#if 0
    /**/ if (ext == "comp") return glCreateShader(GL_COMPUTE_SHADER);
    else if (ext == "frag") return glCreateShader(GL_FRAGMENT_SHADER);
    else if (ext == "geom") return glCreateShader(GL_GEOMETRY_SHADER);
    else if (ext == "vert") return glCreateShader(GL_VERTEX_SHADER);
#endif

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
        exit(-1);
    }

    return shader;
}

unsigned shader2(const char *vs, const char *fs, const char *attributes) {
    GLuint vert = shader_generate(GL_VERTEX_SHADER, vs);
    GLuint frag = shader_generate(GL_FRAGMENT_SHADER, fs);
  //GLuint geom = shader_generate(GL_GEOMETRY_SHADER, gs);
    GLuint program = 0;

    if( vert && frag ) {
        program = glCreateProgram();

        glAttachShader(program, vert);
        glAttachShader(program, frag);
        // glAttachShader(program, geom);

        if( attributes ) {
            char attribute[128] = {0};
            int i = 0;
            for( const char *substr = strstr(attributes, ","); substr ; ) {
                const char *nextstr = substr ? strstr(substr+1, ",") : 0;
                sprintf(attribute, "%.*s", nextstr ? (int)(nextstr - substr) : (int)strlen(substr), substr );
                substr = nextstr;

                glBindAttribLocation(program, i++, attribute);
            }
        }

        glLinkProgram(program);

        GLint status = GL_FALSE, length;
        glGetProgramiv(program, GL_LINK_STATUS, &status);
        if (status == GL_FALSE) {
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
            GLchar *buf = (GLchar*)MALLOC(length);
            glGetProgramInfoLog(program, length, NULL, buf);
            fprintf(stderr, "Error: Shader/program link: %s\n", buf);
            FREE(buf);
            exit(-1);
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

unsigned shader(const char *vs, const char *fs) {
    return shader2(vs, fs, 0);
}

void shader_bind(unsigned shader) {
    glUseProgram(shader);
}

#endif
