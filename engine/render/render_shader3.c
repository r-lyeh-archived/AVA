// Original code by @sgorsten (unlicense)
// - rlyeh, public domain.

#ifndef SHADER3_H
#define SHADER3_H

API unsigned compile_shader(unsigned type, int num_sources, ...);
API unsigned make_program(unsigned shader1, unsigned shader2);
API void delete_program(unsigned program);

API void use(unsigned program);

API void bind_texture(unsigned program, const char *name, unsigned texture);
API void uniform1f(unsigned program, const char *name, float scalar); 
API void uniform2f(unsigned program, const char *name, vec2 vec); 
API void uniform3f(unsigned program, const char *name, vec3 vec); 
API void uniform4f(unsigned program, const char *name, vec4 vec); 
API void uniform16f(unsigned program, const char *name, mat44 mat); 

#endif

#ifdef SHADER3_C
#pragma once

unsigned compile_shader(unsigned type, int num_sources, ...) {
    va_list v;
    va_start(v, num_sources);

        enum { max_sources = 16 };
        GLsizei count = 0;
        GLint lengths[max_sources] = {0};
        const GLchar* sources[max_sources] = {0};

        for( int i = 0; i < num_sources; ++i ) {
            sources[i] = va_arg(v, const GLchar *);
            lengths[i] = strlen(sources[i]);
            if(++count == max_sources) break;
        }

        const GLuint shader = glCreateShader(type == 'v' ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER);
        glShaderSource(shader, count, sources, lengths);
        glCompileShader(shader);

        GLint compile_status;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compile_status);
        if( compile_status != GL_TRUE ) {
            GLint info_log_length;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &info_log_length);

            GLchar *info_log = (GLchar*)calloc(1,info_log_length+1);
            glGetShaderInfoLog(shader, info_log_length, NULL, info_log);
            glDeleteShader(shader);

            for( int i = 0; i < num_sources; ++i ) {
                fprintf(stderr, "// [%d/%d]\n%s\n", i, num_sources, sources[i]);
            }

            PANIC("compile_shader(...) failed with log:\n%s\n", info_log);
            free(info_log);
        }

    va_end(v);
    return shader;
}




unsigned make_program(unsigned shader1, unsigned shader2) {
    GLuint program = glCreateProgram();
    if( shader1 ) glAttachShader(program, shader1);
    if( shader2 ) glAttachShader(program, shader2);
    glLinkProgram(program);

    GLint link_status;
    glGetProgramiv(program, GL_LINK_STATUS, &link_status);
    if( link_status != GL_TRUE ) {
        GLint info_log_length;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &info_log_length);

        char info_log[2048]; // [info_log_length];
        glGetProgramInfoLog(program, 2048, NULL, info_log);
        PANIC("%s", info_log);
    }
    return program;
}

void delete_program(unsigned program) {
    if(program) glDeleteProgram(program);
}

void use(unsigned program) {
    glUseProgram(program);
}

void bind_texture(unsigned program, const char *name, GLuint texture) {
    const GLint location = glGetUniformLocation(program, name);
    if( location >= 0 ) {
        GLint binding;
        glGetUniformiv(program, location, &binding);
        glBindTextureUnit(binding, texture);
    }
}

void uniform1f(unsigned program, const char *name, float scalar) { 
    const GLint location = glGetUniformLocation(program, name);
    if( location >= 0 ) glProgramUniform1f(program, location, scalar);
}
void uniform2f(unsigned program, const char *name, vec2 vec) { 
    const GLint location = glGetUniformLocation(program, name);
    if( location >= 0 ) glProgramUniform2fv(program, location, 1, &vec.x);
}
void uniform3f(unsigned program, const char *name, vec3 vec) { 
    const GLint location = glGetUniformLocation(program, name);
    if( location >= 0 ) glProgramUniform3fv(program, location, 1, &vec.x);
}
void uniform4f(unsigned program, const char *name, vec4 vec) { 
    const GLint location = glGetUniformLocation(program, name);
    if( location >= 0 ) glProgramUniform4fv(program, location, 1, &vec.x);
}
void uniform16f(unsigned program, const char *name, mat44 mat) { 
    const GLint location = glGetUniformLocation(program, name);
    if( location >= 0 ) glProgramUniformMatrix4fv(program, location, 1, GL_FALSE, mat);
}

#endif
