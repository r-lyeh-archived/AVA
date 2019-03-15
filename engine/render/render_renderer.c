#ifndef RENDERER_H
#define RENDERER_H
#include <stdbool.h>
#include <stdint.h>
#include "render_opengl.c" // opengl, render, logger, image, network, timer, window, filesys, 
#include "engine.h" // core/math/math.c

typedef struct renderer_t {
    GLuint texture;
    GLuint shader;
    GLuint u_viewProjMatrix;
    GLuint u_worldMatrix;
    GLuint u_texture;
} renderer_t;

typedef renderer_t material_t;

API void renderer_enable(renderer_t *r, float *camproj);
API void renderer_destroy(renderer_t *r);

// -----------------------------------------------------------------------------

typedef struct transform_t {
    vec3 position;
    vec3 rotation;
    vec3 scale;
    mat44 matrix;
} transform_t;

typedef struct renderable_t {
    transform_t tf;
    int elementCount;
    int smallIndex;
    GLuint vao;
    GLuint ibo16, ibo32;
    GLuint vbo[16];
    GLuint tick;
    GLuint age;    //
    GLuint hidden; //alpha;
} renderable_t;

API renderable_t*renderable(renderable_t *r, int flags);
API void renderable_draw(renderable_t *r);
API void renderable_destroy( renderable_t *r );

API void draw(renderable_t *r, float *transform);

// gpu timer

API int64_t gputime();

#endif

// -----------------------------------------------------------------------------

#ifdef RENDERER_C
#pragma once

renderer_t *g_r = 0;

void renderer_enable(renderer_t *r, float *camproj) {
    // mark renderer as global renderer
    g_r = r;

    // set draw state
    glDisable(GL_CULL_FACE);
    glDepthMask(GL_TRUE);
    glEnable(GL_DEPTH_TEST); // @todo: disable for ui/hud
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // set proj matrix
    glUseProgram(r->shader);
    glUniformMatrix4fv(r->u_viewProjMatrix, 1, GL_FALSE, camproj);

    // set atlas texture
    int unit = 0;
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, r->texture);
    glUniform1i(r->u_texture, unit);
}

void renderer_destroy(renderer_t *r) {
    glDeleteTextures(1, &r->texture);
    glDeleteProgram(r->shader);

    r->texture = 0;
    r->shader = 0;
}

// ----------------------------------------------------------------------------

renderable_t*
renderable(renderable_t *r, int flags){
    memset(r, 0, sizeof(renderable_t));

    return r;
}

void draw(renderable_t *r, float *transform) {
    r->tick++;
    if( r->hidden || (r->age && r->tick > r->age) ) {
        return;
    }

    // set transform
    glUniformMatrix4fv(g_r->u_worldMatrix, 1, GL_FALSE, transform);

    renderable_draw(r);
}


int64_t gputime() {
    GLint64 t = 123456789;
    glGetInteger64v(GL_TIMESTAMP, &t);
    return (int64_t)t;
}

#endif
