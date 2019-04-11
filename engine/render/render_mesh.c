#ifndef MESH_H
#define MESH_H

//#include <stdint.h>
//#include "render_renderer_DEPRECATED.c"

/*
enum {
    MESH_TRIANGULATE,
    MESH_FLIP_UV,
    MESH_GENERATE_SMOOTH_NORMALS,
    MESH_TRANSLATE_TO_ORIGIN,
    MESH_SCALE_x10,
    MESH_SCALE_x100,
    MESH_SCALE_x1000,
    MESH_ROTATE_90,
    MESH_ROTATE_180,
    MESH_ROTATE_270,

    MESH_CONVERT_INPLACE_TO_ROOT_ANIMATION, // generate inplace->root anim conversion (requires left-foot, right-foot bones)
    MESH_CONVERT_ROOT_TO_INPLACE_ANIMATION, // generate root->inplace anim conversion
    MESH_BAKE_AO,

    MESH_LIMITBONEWEIGHTS,                  // 4 ?
    MESH_IMPROVECACHELOCALITY,              
};
*/

/*
#define BIs blend indexes
#define BWs blend weights
GLubyte blendindex[4];
GLubyte blendweight[4];
*/

enum {
    // ibo type
    INDEX_UNDEFINED = 0,
    INDEX_16 = 16,
    INDEX_32 = 32,
    // order layout
    ATT_POSITION = 0,
    ATT_COLOR = 1,
    ATT_UV = 2,
    ATT_NORMAL = 3,
    ATT_TANGENT = 4,
    ATT_BITANGENT = 5,
    // number of components
    LEN_POSITION = 3,
    LEN_COLOR = 4,
    LEN_UV = 2,
    LEN_NORMAL = 3,
    LEN_TANGENT = 3,
    LEN_BITANGENT = 1, // (*)
    // * vec3 Bitangent can be reconstructed from vec1 (sign of W) with:
    //    normal = (gWorld * vec4(normal, 0.0f)).xyz;
    //    tangent = (gWorld * vec4(tangent, 0.0f)).xyz;
    //    bitangent = w * cross(normal, tangent);
    // * and in that case, bitangent[0] could be stored in [3] of vec4 tangent;
    VERTEX_UNDEFINED = 0,
    VERTEX_INTERLEAVED = 1,
    VERTEX_P = 0x100<<0,
    VERTEX_C = 0x100<<1,
    VERTEX_U = 0x100<<2,
    VERTEX_N = 0x100<<3,
    VERTEX_T = 0x100<<4,
    VERTEX_B = 0x100<<5,
};

typedef struct vertex_p {
    float position[LEN_POSITION];
} vertex_p;

typedef struct vertex_pc {
    float position[LEN_POSITION];
  uint8_t color[LEN_COLOR];
} vertex_pc;

typedef struct vertex_pcu {
    float position[LEN_POSITION];
  uint8_t color[LEN_COLOR];
    float texcoord[LEN_UV];
} vertex_pcu;

typedef struct vertex_pcun {
    float position[LEN_POSITION];
  uint8_t color[LEN_COLOR];
    float texcoord[LEN_UV];
    float normal[LEN_NORMAL];
} vertex_pcun;

typedef struct vertex_pcunt {
    float position[LEN_POSITION];
  uint8_t color[LEN_COLOR];
    float texcoord[LEN_UV];
    float normal[LEN_NORMAL];
    float tangent[LEN_TANGENT];
} vertex_pcunt;

typedef struct vertex_pcuntb {
    float position[LEN_POSITION];
  uint8_t color[LEN_COLOR];
    float texcoord[LEN_UV];
    float normal[LEN_NORMAL];
    float tangent[LEN_TANGENT];
    float bitangent[LEN_BITANGENT];
} vertex_pcuntb;

typedef vertex_pcuntb vertex;

typedef struct buffer {
    void *pointer;
    size_t size;
} buffer;

typedef struct mesh_t {
    int elementCount;
    int smallIndex;
    GLuint vao;
    GLuint ibo16, ibo32;
    GLuint vbo[16];
} mesh_t;

API mesh_t *mesh( struct mesh_t* r, int flags, int num_elems, const buffer *buffers );
API mesh_t *mesh_loadfile( struct mesh_t* r, const char *filename );

#endif

// ----------------------------------------------------------------------------

#ifdef MESH_C
#pragma once
//#include "render_renderer.c"
//#include "render_mesh.c"
#include <float.h>
#include "engine.h" // filesys, math



mesh_t *
mesh( mesh_t* r, int flags, int num_elems, const buffer *buffers ) {
    GLenum E = GL_ELEMENT_ARRAY_BUFFER;
    GLenum V = GL_ARRAY_BUFFER;
    GLenum S = GL_STATIC_DRAW;

    memset(r, 0, sizeof(mesh_t)); // comment?

    struct datatype {
        int mask;
        int elements;
        GLenum format;
        GLenum normalized;
    } datatype[] = {
        { VERTEX_P, LEN_POSITION, GL_FLOAT, GL_FALSE },
        { VERTEX_C, LEN_COLOR, GL_UNSIGNED_BYTE, GL_TRUE },
        { VERTEX_U, LEN_UV, GL_FLOAT, GL_FALSE },
        { VERTEX_N, LEN_NORMAL, GL_FLOAT, GL_FALSE },
        { VERTEX_T, LEN_TANGENT, GL_FLOAT, GL_FALSE },
        { VERTEX_B, LEN_BITANGENT, GL_FLOAT, GL_FALSE },
        0
    };
    int stride = 0, offset[16] = { 0 };
    for (int i = 0; i < sizeof(datatype) / sizeof(0[datatype]); ++i) {
        if (flags & datatype[i].mask) {
            float component = datatype[i].elements * (datatype[i].format == GL_FLOAT ? sizeof(float) : sizeof(uint8_t));
            stride += component;
            offset[i] = stride - component;
        }
    }
    r->elementCount = num_elems ? num_elems : buffers[0].size / stride;

    glGenVertexArrays(1, &r->vao);
    glBindVertexArray(r->vao);

    if( flags & (16|32) ) {
        glGenBuffers(1, flags & 16 ? &r->ibo16 : &r->ibo32);
        glBindBuffer(E, flags & 16 ?  r->ibo16 :  r->ibo32);
        glBufferData(E, buffers->size, buffers->pointer, S);
        ++buffers;
    }


    if( flags & VERTEX_INTERLEAVED ) {
        glGenBuffers(1, &r->vbo[0]);
        glBindBuffer(V, r->vbo[0]);
        glBufferData(V, buffers->size, buffers->pointer, S);
        ++buffers;

        for( int i = 0; i < sizeof(datatype)/sizeof(0[datatype]); ++i ) {
            if( flags & datatype[i].mask ) {
                glEnableVertexAttribArray(i);
                glVertexAttribPointer(i, datatype[i].elements, datatype[i].format, datatype[i].normalized, stride, ((char*)0+offset[i]));

                //printf("enabled att#%d (len:%d%c) (norm:%d)\n", i, datatype[i].elements, datatype[i].format == GL_FLOAT ? 'f':'u', datatype[i].normalized);
            }
        }
    } else {
        for( int i = 0; i < sizeof(datatype)/sizeof(0[datatype]); ++i ) {
            if( flags & datatype[i].mask ) {
                glGenBuffers(1, &r->vbo[i]);
                glBindBuffer(V, r->vbo[i]);
                glBufferData(V, buffers->size, buffers->pointer, S);
                ++buffers;

                glEnableVertexAttribArray(i);
                glVertexAttribPointer(i, datatype[i].elements, datatype[i].format, datatype[i].normalized, 0, (char*)0);

                //printf("enabled att#%d (len:%d%c) (norm:%d)\n", i, datatype[i].elements, datatype[i].format == GL_FLOAT ? 'f':'u', datatype[i].normalized);
            }
        }
    }

    glBindVertexArray(0);
    return r;
}

// -----------------------------------------------------------------------------

int vertex_size( int vertex_type ) {
    if( vertex_type & VERTEX_B ) return sizeof(vertex_pcuntb);
    if( vertex_type & VERTEX_T ) return sizeof(vertex_pcunt);
    if( vertex_type & VERTEX_N ) return sizeof(vertex_pcun);
    if( vertex_type & VERTEX_U ) return sizeof(vertex_pcu);
    if( vertex_type & VERTEX_C ) return sizeof(vertex_pc);
    if( vertex_type & VERTEX_P ) return sizeof(vertex_p);
    return 0;
}

// ----------------------------------------------------------------------------

int index_size( int index_type ) {
    if( index_type == INDEX_16 ) return sizeof(uint16_t);
    if( index_type == INDEX_32 ) return sizeof(uint32_t);
    return 0;
}

#endif
