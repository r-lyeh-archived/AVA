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

// ---

typedef struct texture3 {
    GLuint id;
    int width, height, depth;
    int target, internal_format, unit;
} texture3;

API texture3 texture3_create(int unit, int w, int h, int d, int internal_format, int format, int type, int filter, int wrap, void *data);
API void     texture3_bind(texture3 t);
API void     texture3_unbind(texture3 t);
API texture2 texture3_to2( texture3 t );

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

// ---

texture2 texture3_to2( texture3 t ) {
    texture2 s = { "", (int)t.id, (int)t.width, (int)t.height, (unsigned)t.target, (unsigned)t.internal_format };
    return s;
}

void texture3_bind(texture3 t) {
    glActiveTexture(GL_TEXTURE0 + t.unit); glBindTexture(t.target, t.id);
}
void texture3_unbind(texture3 t) {
    glActiveTexture(GL_TEXTURE0 + t.unit); glBindTexture(t.target, 0);
}

texture3 texture3_create(int unit, int w, int h, int d, int internal_format, int format, int type, int filter, int wrap, void *data) {
    texture3 t = {0};
    t.target = (d <= 1) ? GL_TEXTURE_2D : GL_TEXTURE_3D;
    t.width = w;
    t.height = h;
    t.depth = d;
    if (!t.id) glGenTextures(1, &t.id);
    t.unit = unit;
    t.internal_format = internal_format;

    texture3_bind(t);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(t.target, GL_TEXTURE_MAG_FILTER, filter);
    glTexParameteri(t.target, GL_TEXTURE_MIN_FILTER, filter);
    glTexParameteri(t.target, GL_TEXTURE_WRAP_S, wrap);
    glTexParameteri(t.target, GL_TEXTURE_WRAP_T, wrap);
    if (t.target == GL_TEXTURE_2D) {
        glTexImage2D(t.target, 0, internal_format, w, h, 0, format, type, data);
    } else {
        glTexParameteri(t.target, GL_TEXTURE_WRAP_R, wrap);
        glTexImage3D(t.target, 0, internal_format, w, h, d, 0, format, type, data);
    }

    texture3_unbind(t);
    return t;
}

#endif
