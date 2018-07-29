#ifndef MESH_H
#define MESH_H
#include <stdint.h>
#include "render_renderer.c"

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

renderable_t *mesh( void *renderable, int flags, int num_elems, const buffer *buffers );
renderable_t *mesh_loadfile( void *renderable, const char *filename );

#endif

// ----------------------------------------------------------------------------

#ifdef MESH_C
#pragma once
#include "render_renderer.c"
#include "../math/math.c"
#include <float.h>
#include "render_mesh.c"
#include "../framework/filesys_.c"

renderable_t *
mesh( void *renderable, int flags, int num_elems, const buffer *buffers ) {
    GLenum E = GL_ELEMENT_ARRAY_BUFFER;
    GLenum V = GL_ARRAY_BUFFER;
    GLenum S = GL_STATIC_DRAW;

    renderable_t *r = (renderable_t*) renderable;
    memset(r, 0, sizeof(renderable_t)); // comment?

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

                printf("enabled att#%d (len:%d%c) (norm:%d)\n", i, datatype[i].elements, datatype[i].format == GL_FLOAT ? 'f':'u', datatype[i].normalized);
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

                printf("enabled att#%d (len:%d%c) (norm:%d)\n", i, datatype[i].elements, datatype[i].format == GL_FLOAT ? 'f':'u', datatype[i].normalized);
            }
        }
    }

    glBindVertexArray(0);
    return r;
}

void renderable_draw(void *renderable) {
    renderable_t *r = (renderable_t*)renderable;

    glBindVertexArray(r->vao);
    if( r->ibo16 ) glDrawElements(GL_TRIANGLES, r->elementCount, GL_UNSIGNED_SHORT, (char*)0); // with index16
    if( r->ibo32 ) glDrawElements(GL_TRIANGLES, r->elementCount, GL_UNSIGNED_INT,   (char*)0); // with index32
    else           glDrawArrays(GL_TRIANGLES, 0, r->elementCount);                             // without index
    glBindVertexArray(0);
}

void renderable_destroy( void *renderable ) {
    renderable_t *r = (renderable_t*)renderable;
    for( int i = 0; i < sizeof(r->vbo) / sizeof(r->vbo[0]); ++i ) {
        glDeleteBuffers(1, &r->vbo[i]);
    }
    glDeleteBuffers(1, &r->ibo16);
    glDeleteBuffers(1, &r->ibo32);
    glDeleteVertexArrays(1, &r->vao);

    memset( r, 0, sizeof(renderable));
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

// ----------------------------------------------------------------------------

#if 0 // ok

// @glampert
void aabb_meshgl(mesh_gl *m, vec3 bbmin, vec3 bbmax ) {
    bbmin[0] = bbmin[1] = bbmin[2] = FLT_MAX;
    bbmax[0] = bbmax[1] = bbmax[2] = FLT_MIN;

    float *v = (float *)m->vertex_data;
    int stride = vertex_size(m->vertex_type);

    int vertex_count = m->vertex_bytes / vertex_size(m->vertex_type);

    for( int i = 0; i < vertex_count; i++ ) {
        for( int j = 0; j < 3; j++ ) {
            if( v[j] < bbmin[j] ) bbmin[j] = v[j];
            if( v[j] > bbmax[j] ) bbmax[j] = v[j];
        }
        v += stride;
    }
}

#endif

/*

mesh_gl *loadmem_bob_meshgl(mesh_gl *self, const char *data, size_t length) {
}

*/

renderable_t *mesh_loadmem_bob(renderable_t *self, const char *data, size_t length) {
    // .bob format (little-endian):
    // [bytes_vp:4][bytes_vt:4][bytes_vn:4][bytes_idx:4][P3f...][U2f...][N3f...][IDX32...]
    uint32_t bytes_vp = ((uint32_t*)data)[0];
    uint32_t bytes_vt = ((uint32_t*)data)[1];
    uint32_t bytes_vn = ((uint32_t*)data)[2];
    uint32_t bytes_id = ((uint32_t*)data)[3];
    // extract pointer offsets in data
    void *vp = (void*)&data[4*4];
    void *vt = (void*)&data[4*4 + bytes_vp];
    void *vn = (void*)&data[4*4 + bytes_vp + bytes_vt];
    void *id = (void*)&data[4*4 + bytes_vp + bytes_vt + bytes_vn];
    // convert indices ptn[...] to p[...]
    uint32_t *id2 = (uint32_t*)malloc(bytes_id);
    uint32_t count_id = bytes_id / sizeof(0[id2]);
    for( int i = 0; i < count_id; i += 3 ) {
        id2[ i / 3 ] = ((uint32_t*)id)[ i ];
    }
    id = id2;
    bytes_id /= 3;
    // upload
    buffer buffers[] = {
        { id, bytes_id },
        { vp, bytes_vp },
        { vt, bytes_vt },
        { vn, bytes_vn },
    };
    self = mesh(self, VERTEX_P|VERTEX_U|VERTEX_N|32, bytes_id / sizeof(uint32_t), buffers);
    free(id2);
    return self;
}

renderable_t *mesh_loadmem_ibo(renderable_t *self, const char *data, size_t length) {

#if 1
    // convert p3n3u2 stream (.ibo) to p3u2n3 stream
    // num_tris = sizeof(ibo_file) / (8*sizeof(float))

    const float *src = (const float *)data;
    float *buf = (float*)malloc( length * 2 ), *dst = buf;
    for( int i = 0, end = length; i < end; i += 8 * sizeof(float) ) {
        float v[8];
        memcpy( &v, src, 8 * sizeof(float));
        src += 8;

        // p3
        *dst++ = v[0];
        *dst++ = v[1];
        *dst++ = v[2];
        // u2
        *dst++ = v[6];
        *dst++ = v[7];
        // n3
        *dst++ = v[3];
        *dst++ = v[4];
        *dst++ = v[5];
    }

    buffer buffers[] = {
        { (void*)buf, sizeof(float)*(dst-buf) },
    };

    self = mesh(self, VERTEX_P|VERTEX_U|VERTEX_N|VERTEX_INTERLEAVED|0, 0, buffers);

    free(buf);
#else
    // load alt p3u2n3 stream (.ibo)
    // num_tris = sizeof(ibo_file) / (8*sizeof(float))

    buffer buffers[] = {
        { (void*)buf, length },
    };

    self = mesh(self, VERTEX_P|VERTEX_U|VERTEX_N|VERTEX_INTERLEAVED|0, 0, buffers);
#endif

    return self;
}

renderable_t *mesh_loadfile(renderable_t *self, const char *pathfile) {
    return mesh_loadmem_bob(self, file_read_(pathfile), file_size_(pathfile));
}

#endif
