#ifndef SHADER2_H
#define SHADER2_H

// shader

// API unsigned shader( const char *vert, const char *frag );
API unsigned shader2(const char *vp, const char *fp, int num_attributes, const char **attributes);

API void shader_bind_int1(unsigned program, const char *uniform, int value);
API void shader_bind_texture(unsigned program, unsigned texture, unsigned unit);
API void shader_push_strip(unsigned program, unsigned vertex_vao, unsigned first, unsigned count);

API void shader_bind_int(unsigned program, const char *uniform, int value);
API void shader_bind_float(unsigned program, const char *uniform, float f);
API void shader_bind_vec2(unsigned program, const char *uniform, float v[2]);
API void shader_bind_vec3(unsigned program, const char *uniform, float v[3]);
API void shader_bind_vec4(unsigned program, const char *uniform, float v[4]);
API void shader_bind_mat44(unsigned program, const char *uniform, float m[16]);

API const char *shader_default_diffuse_fs();
API const char *shader_default_fullscreen_vs();

#endif

// ----------------------------------------------------------------------------

#ifdef SHADER2_C
#pragma once
#include "render_opengl.c"
#include <stdlib.h>

#define UNIFORM_LOC(program,name) (name[0] >= '0' && name[0] <= '9' ? atoi(name) : glGetUniformLocation(program,name))

void shader_bind_int(unsigned program, const char *name, int i)        { glUseProgram(program); glUniform1i(UNIFORM_LOC(program,name), i); }
void shader_bind_float(unsigned program, const char *name, float f)    { glUseProgram(program); glUniform1f(UNIFORM_LOC(program,name), f); }
void shader_bind_vec2(unsigned program, const char *name, float v[2])  { glUseProgram(program); glUniform2fv(UNIFORM_LOC(program,name), 1, v); }
void shader_bind_vec3(unsigned program, const char *name, float v[3])  { glUseProgram(program); glUniform3fv(UNIFORM_LOC(program,name), 1, v); }
void shader_bind_vec4(unsigned program, const char *name, float v[4])  { glUseProgram(program); glUniform4fv(UNIFORM_LOC(program,name), 1, v); }
void shader_bind_mat44(unsigned program, const char *name, float m[16]) { glUseProgram(program); glUniformMatrix4fv(UNIFORM_LOC(program,name), 1, GL_FALSE/*GL_TRUE*/, m); }

void shader_bind_int1(GLuint program, const char *uniform, int value) {
    glUseProgram(program);
    glUniform1i(glGetUniformLocation(program, uniform), value);
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

#include <stdint.h>
#include <stdio.h>

static GLuint shader_load(GLenum type, const char *source) {
    GLuint shader = glCreateShader(type);
    if (shader) {
        glShaderSource(shader, 1, &source, NULL);
        glCompileShader(shader);
        GLint compiled;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
        if (compiled) {
            return shader;
        }
        GLint loglen = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &loglen);
        if (loglen) {
            ALLOCA(GLchar, infoLog, loglen);
            glGetShaderInfoLog(shader, loglen, NULL, infoLog);
            fprintf(stderr, "%s\n", infoLog);
        }
        glDeleteShader(shader);
    }
    return 0;
}

unsigned shader2(const char *vp, const char *fp, int num_attributes, const char **attributes) {
    GLuint vertexShader = shader_load(GL_VERTEX_SHADER, vp);
    if( vertexShader ) {
        GLuint fragmentShader = shader_load(GL_FRAGMENT_SHADER, fp);
        if (fragmentShader) {
            GLuint program = glCreateProgram();
            if (program) {
                glAttachShader(program, vertexShader);
                glAttachShader(program, fragmentShader);
                
                for (int i = 0; i < num_attributes; i++) {
                    glBindAttribLocation(program, i, attributes[i]);
                }

                glLinkProgram(program);

                GLint linked;
                glGetProgramiv(program, GL_LINK_STATUS, &linked);
                if (linked) {
                    glDeleteShader(vertexShader);
                    glDeleteShader(fragmentShader);
                    return program;
                }

                GLint infoLen = 0;
                glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLen);
                if (infoLen) {
                    ALLOCA(GLchar, infoLog, infoLen);
                    glGetProgramInfoLog(program, infoLen, NULL, infoLog);
                    fprintf(stderr, "%s\n", infoLog);
                }
                glDeleteProgram(program);
            }
            glDeleteShader(fragmentShader);
        }
        glDeleteShader(vertexShader);
    }
    fprintf(stderr, "Could not create program!\n");
    return 0;
}


#endif
