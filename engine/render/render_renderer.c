#ifndef RENDERER_H
#define RENDERER_H
#include <stdbool.h>
#include <stdint.h>
#include "../framework/opengl.c"
#include "../framework/framework.c" // logger, image, network, timer, window, filesys, 
#include "../framework/render.c"
#include "../core/math/math.c"

typedef struct renderer_t {
    GLuint texture;
    GLuint shader;
    GLuint u_viewProjMatrix;
    GLuint u_worldMatrix;
    GLuint u_texture;
} renderer_t;

typedef renderer_t material_t;

void renderer_enable(void *renderer, float camproj[16]);
void renderer_destroy(void *renderer);

// -----------------------------------------------------------------------------

typedef struct transform_t {
    vec3 position;
    vec3 rotation;
    vec3 scale;
    mat4x4 matrix;
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

renderable_t*renderable(void *renderable, int flags);
void renderable_draw(void *renderable);
void renderable_destroy( void *renderable );

void draw(void *renderable, float transform[16]);

// gpu timer

int64_t gputime();

#endif

// -----------------------------------------------------------------------------

#ifdef RENDERER_C
#pragma once

renderer_t *g_r;

void renderer_enable(void *renderer, float camproj[16]) {
    renderer_t *r = (renderer_t*)renderer;

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

void renderer_destroy(void *renderer) {
    renderer_t *r = (renderer_t*)renderer;

    glDeleteTextures(1, &r->texture);
    glDeleteProgram(r->shader);

    r->texture = 0;
    r->shader = 0;
}

// ----------------------------------------------------------------------------

renderable_t*
renderable(void *renderable, int flags){
    renderable_t *r = (renderable_t*) renderable;
    memset(r, 0, sizeof(renderable_t));

    return r;
}

void draw(void *renderable, float transform[16]) {
    renderable_t *r = (renderable_t*)renderable;

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
