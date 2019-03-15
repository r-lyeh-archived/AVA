#ifndef SHADER2_H
#define SHADER2_H

// shader

// API unsigned shader( const char *vert, const char *frag );

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

void shader_bind_int(unsigned program, const char *name, int i)        { glUseProgram(program); glUniform1i(glGetUniformLocation(program,name), i); }
void shader_bind_float(unsigned program, const char *name, float f)    { glUseProgram(program); glUniform1f(glGetUniformLocation(program,name), f); }
void shader_bind_vec2(unsigned program, const char *name, float v[2])  { glUseProgram(program); glUniform2fv(glGetUniformLocation(program,name), 1, v); }
void shader_bind_vec3(unsigned program, const char *name, float v[3])  { glUseProgram(program); glUniform3fv(glGetUniformLocation(program,name), 1, v); }
void shader_bind_vec4(unsigned program, const char *name, float v[4])  { glUseProgram(program); glUniform4fv(glGetUniformLocation(program,name), 1, v); }
void shader_bind_mat44(unsigned program, const char *name, float m[16]) { glUseProgram(program); glUniformMatrix4fv(glGetUniformLocation(program,name), 1, GL_FALSE/*GL_TRUE*/, m); }

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

#endif
