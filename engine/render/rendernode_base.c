#ifndef BASE_H
#define BASE_H
// #include <engine.h>

#include "render_mesh.c"

typedef struct rendernode {
    struct transform_t {
        vec3 position;
        vec3 rotation;
        vec3 scale;
        mat44 matrix;
    } tf;

    GLuint tick;
    GLuint age;    //
    GLuint hidden; //alpha;

#if 0
    int elementCount;
    int smallIndex;
    GLuint vao;
    GLuint ibo16, ibo32;
    GLuint vbo[16];
#else
    // float *mat44;

    struct mesh_t mesh1;
    struct mesh2 *mesh;
#endif

    struct material *material;
} rendernode;

API void rendernode_create(rendernode *r, int flags);
API void rendernode_draw(rendernode *r, float model[16]);
API void rendernode_destroy(rendernode *r);

#endif

// ----------------------------------------------------------------------------

#ifdef  BASE_C
#pragma once
#include "render_material2.c"
#include "render_mesh.c"
#include "render_mesh2.c"

void rendernode_create(rendernode *r, int flags){
    rendernode c = {0};
    *r = c;
}

void rendernode_draw(rendernode *r, float model[16]) {
    r->tick++;
    if( r->hidden || (r->age && r->tick > r->age) ) {
        return;
    }

    if( r->material && model ) {
        material_sendmodel(r->material, model);
    }

    if(r->mesh) mesh2_render(r->mesh, 0);
    else {
        glBindVertexArray(r->mesh1.vao);
        /**/ if( r->mesh1.ibo16 ) glDrawElements(GL_TRIANGLES, r->mesh1.elementCount, GL_UNSIGNED_SHORT, (char*)0); // with index16
        else if( r->mesh1.ibo32 ) glDrawElements(GL_TRIANGLES, r->mesh1.elementCount, GL_UNSIGNED_INT,   (char*)0); // with index32
        else                      glDrawArrays(GL_TRIANGLES, 0, r->mesh1.elementCount);                             // without index
        glBindVertexArray(0);
    }
}

void rendernode_destroy( rendernode* r ) {
//    if(r->material) material_destroy(r->material);

    if(r->mesh) mesh2_destroy(r->mesh);
    else {
        for( int i = 0; i < sizeof(r->mesh1.vbo) / sizeof(r->mesh1.vbo[0]); ++i ) {
            glDeleteBuffers(1, &r->mesh1.vbo[i]);
        }
        glDeleteBuffers(1, &r->mesh1.ibo16);
        glDeleteBuffers(1, &r->mesh1.ibo32);
        glDeleteVertexArrays(1, &r->mesh1.vao);
    }

    memset( r, 0, sizeof(rendernode));
}

#endif
