#ifndef TEXTURE2_H
#define TEXTURE2_H

typedef struct texture2 {
    const char *debug;

    int id, width, height;
    unsigned target; // GL_TEXTURE_2D, 3D, CUBEMAP
    unsigned format; // pixel format
} texture2;

API unsigned texture_make_rgb(int texture_width, int texture_height, unsigned char *texture_data);
API void     texture_patch_rgb(unsigned texture, int x, int y, int w, int h, unsigned char *texture_data);

#endif

// ----------------------------------------------------------------------------

#ifdef TEXTURE2_C
#pragma once
#include "render_opengl.c"

unsigned texture_make_rgb(int texture_width, int texture_height, unsigned char *texture_data) {
    GLuint texture;
    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture_width, texture_height, 0, GL_RGB, GL_UNSIGNED_BYTE, texture_data);
    return texture;
}

void texture_patch_rgb(unsigned texture, int x, int y, int w, int h, unsigned char *texture_data) {
    GLint mip = 0;
    glBindTexture(GL_TEXTURE_2D, (GLuint)texture);
    glTexSubImage2D(GL_TEXTURE_2D, mip, x, y, w, h, GL_RGB, GL_UNSIGNED_BYTE, texture_data);
}

#endif
