API void quad( void *renderable );

// ----------------------------------------------------------------------------

#ifdef MESH_C
#pragma once
#include "render_mesh.c"
#include "render_renderer.c"

void quad( void *renderable ) {
    renderable_t *r = (renderable_t*)renderable;

    // A--B ; quad CDAB becomes triangle CAB and CBD.
    // | /| 
    // |/ | 
    // C--D 

    float vertices[] = {
        -1, -1, 0, // C
        -1,  1, 0, // A
         1,  1, 0, // B
        -1, -1, 0, // C
         1,  1, 0, // B
         1, -1, 0, // D
    };

    uint32_t colors[] = {
        0xFF0000FF, // C
        0xFF00FF00, // A
        0xFFFF0000, // B
        0xFF0000FF, // C
        0xFFFF0000, // B
        0xFFFFFFFF, // D
    };

    float uvs[] = {
        0, 1,
        0, 0,
        1, 0,
        0, 1,
        1, 0,
        1, 1,
    };

    buffer buffers[] = {
        { vertices, sizeof(vertices) },
        { colors, sizeof(colors) },
        { uvs, sizeof(uvs) },
    };

    mesh( r, VERTEX_P|VERTEX_C|VERTEX_U, 6, buffers );
}

#endif
