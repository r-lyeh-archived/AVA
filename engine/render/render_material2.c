#ifndef MATERIAL2_H
#define MATERIAL2_H
#include <stdbool.h>
#include <stdint.h>
#include "render_opengl.c" // opengl, render, logger, image, network, timer, window, filesys, 
// #include "engine.h" // core/math/math.c

typedef struct material {
    GLenum wireframe;
    GLenum two_sided;
    GLenum depth_enable_write;
    GLenum depth_enable_test;
    GLenum depth_func;
    GLenum alpha_enable;
    GLenum alpha_src;
    GLenum alpha_dst;

    GLuint texture;
    GLuint shader;
    GLint u_VP; // @todo: MVP (modelviewproj)
    GLint u_M; //worldMatrix;
    GLint u_texture;
} material;

API void material_create(material *m);
API void material_enable(material *m, float projview[16]);
API void material_sendmodel(material *m, float model[16]);
API void material_destroy(material *m);

#endif

// -----------------------------------------------------------------------------

#ifdef MATERIAL2_C
#pragma once

static array(material) materials = 0;

void material_create(material *m) {
    material n = {0};
    *m = n;

    m->wireframe = false;
    //m->face_ccw = 1; // cw, ccw
    m->two_sided = true; // GL_FALSE;
    m->depth_enable_write = true;
    m->depth_enable_test = true;
    m->depth_func = GL_LESS;
    m->alpha_enable = GL_FALSE;
    m->alpha_src = GL_ONE; //GL_SRC_ALPHA;
    m->alpha_dst = GL_ZERO; //GL_ONE_MINUS_SRC_ALPHA;

    // for regular alpha,
    //   additive(GL_ONE, GL_ONE)
    //   decal(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)
    //   modulate(GL_DST_COLOR, GL_ZERO)
    //or for premultiplied alpha,
    //   premultiply(GL_ONE, GL_ONE_MINUS_SRC_ALPHA); 
    //   then, in shader: vec4(rgb * alpha, alpha) for normal blending, and vec4(rgb, 0) for additive.

    // depth state enabled
    // col src one
    // col dst zero
    // alpha src one

    m->u_VP = -1;
    m->u_M = -1;
    m->u_texture = -1;
}

void material_enable(material *m, float projview[16]) {
    // push material into stack
    // array_push(materials, m);

    // set draw state
    glPolygonMode(GL_FRONT_AND_BACK, m->wireframe ? GL_LINE : GL_FILL);

    m->two_sided ? glDisable(GL_CULL_FACE) : glEnable(GL_CULL_FACE), glCullFace(GL_BACK);

    glDepthMask(m->depth_enable_write ? GL_TRUE : GL_FALSE);
    m->depth_enable_test ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
    glDepthFunc(m->depth_func);

    !m->alpha_enable ? glDisable(GL_BLEND) : glEnable(GL_BLEND),glBlendFunc(m->alpha_src, m->alpha_dst);

    //glFrontFace(GL_CCW);

    // set shader
    glUseProgram(m->shader);

    // set matrices
    if(projview && m->u_VP >= 0) {
        glUniformMatrix4fv(m->u_VP, 1, GL_FALSE, projview);
    }

    // set atlas texture
    if( m->texture > 0 && m->u_texture >= 0 ) {
        int unit = 0;
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_2D, m->texture);
        glUniform1i(m->u_texture, unit);
    }
}

void material_sendmodel(material *m, float model[16]) {
    if(model && m->u_M >= 0) glUniformMatrix4fv(m->u_M, 1, GL_FALSE, model);
}

void material_destroy(material *m) {
    glDeleteTextures(1, &m->texture);
    glDeleteProgram(m->shader);

    m->texture = 0;
    m->shader = 0;
}

#endif


// -----------------------------------------------------------------------------

