#ifndef MESH_H
#define MESH_H

#include "render/render_opengl.c"
#include "math.h"

#ifndef offsetof
#error
#endif

/* char texture_usage enum = (d)iffuse, (n)ormal, (s)pecular... */

// vertex format (p|n|t|c|w|a|i)(1|2|3|4)[f|u|b]
// (p)osition
// (n)ormal
// (t)excoord
// (c)olor
// (w)eight
// w(e)ight ids
// t(a)ngent
// b(i)tangent
// vec(1)
// vec(2)
// vec(3)
// vec(4)
// (f)loating
// (u)nsigned int
// (b)yte

enum MESH2_FLAGS {
    MESH2_STATIC = 0, // , DYNAMIC, STREAM // zero|single|many updates per frame
    MESH2_STREAM = 1,
    MESH2_TRIANGLE_STRIP = 2,
};

typedef struct material3 {
    GLuint texture_id[8], texture_usage[8], texture_count;
} material3;

#pragma pack(1)
typedef struct mesh {
    GLuint vao, vbo, ibo;
    GLuint vertex_count, index_count;
    union {
    GLuint texture_id[1];
    GLuint material_id;
    struct material *material;
    };
    unsigned flags;
    vec3 minbb, maxbb; // center+extents instead ?
    char name[64 - sizeof(GLuint) * 5 - sizeof(struct material*) - sizeof(unsigned) - sizeof(vec3) * 2]; // char *name; instead?
} mesh;
#pragma pack()


API void   mesh_create(mesh* m, const char *format, int vertex_stride_optional,int vertex_count,void *vertex_data, int index_count,void *index_data, int flags);
API void   mesh_update(mesh* m, const char *format, int vertex_stride_optional,int vertex_count,void *vertex_data, int index_count,void *index_data, int flags);
API void   mesh_render(mesh* m, unsigned program); // , int instanceCount = 1);
API void   mesh_destroy(mesh* m);

API void   mesh_make_quad( struct mesh *m );

#endif


#ifdef MESH_C
#pragma once
#include <stdlib.h>
#include <stdarg.h>

#define STS_VERTEX_CACHE_OPTIMIZER_IMPLEMENTATION
#include "3rd/sts_vertex_cache_optimizer.h"

static
unsigned int *index_optimize(unsigned int *index_data, int index_count, int vertex_count) {
    float before = stsvco_compute_ACMR( index_data, index_count, 8 );
    stsvco_optimize( index_data, index_count, vertex_count, 32 );
    float after = stsvco_compute_ACMR( index_data, index_count, 8 );
    printf("vertex optimization %f -> %f\n", before, after);
    return index_data;
}

typedef int mesh_static_assert[ sizeof(mesh) == 64 ];

void mesh_create(mesh* m, const char *format, int vertex_stride,int vertex_count,void *vertex_data, int index_count,void *index_data, int flags) {
    // assert( is_any_shader_bound() ); 

    mesh z = {0};
    *m = z;

    mesh_update(m, format, vertex_stride,vertex_count,vertex_data, index_count,index_data, flags);
}

void mesh_update(mesh* m, const char *format, int vertex_stride,int vertex_count,void *vertex_data, int index_count,void *index_data, int flags) {
    m->flags = flags;

    // setup
    unsigned sizeof_index = sizeof(GLuint);
    unsigned sizeof_vertex = 0;
    m->index_count = index_count;
    m->vertex_count = vertex_count;

    // iterate vertex attributes { position, normal + uv + tangent + bitangent + ... }
    struct vertex_descriptor {
        int vertex_type, num_attribute, num_components;
        int stride, offset;
    } descriptor[16] = {0}, *dc = &descriptor[0];

    do switch( *format ) {
        break; case '0': dc->num_components = 0;
        break; case '1': dc->num_components = 1;
        break; case '2': dc->num_components = 2;
        break; case '3': dc->num_components = 3;
        break; case '4': dc->num_components = 4;
        break; case 'f': dc->vertex_type = GL_FLOAT;
        break; case 'u': dc->vertex_type = GL_UNSIGNED_INT;
        break; case 'b': if(format[-1] >= '0' && format[-1] <= '9') dc->vertex_type = GL_UNSIGNED_BYTE; //else bitangent.
        break; case ' ': case '\0': 
            if (!dc->vertex_type) dc->vertex_type = GL_FLOAT;
            dc->offset = sizeof_vertex;
            sizeof_vertex += (dc->stride = dc->num_components * (dc->vertex_type == GL_UNSIGNED_BYTE ? 1 : 4));
            ++dc;
        break; default: if( !strchr("pntcwai", *format) ) PANIC("unsupported vertex type '%c'", *format);
    } while (*format++);

    if(vertex_stride > 0) sizeof_vertex = vertex_stride;

    // layout
    if(!m->vao) glGenVertexArrays(1, &m->vao);
    glBindVertexArray(m->vao);

    // index data
    if( index_data && index_count ) {
        m->index_count = index_count;

        if( 0 )
        if( vertex_count )
        index_optimize(index_data, index_count, vertex_count);

        if(!m->ibo) glGenBuffers(1, &m->ibo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m->ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, m->index_count * sizeof_index, index_data, flags & MESH2_STREAM ? GL_STREAM_DRAW : GL_STATIC_DRAW);
    }

    // vertex data
    if( vertex_data && vertex_count ) {
        m->vertex_count = vertex_count;

        if(!m->vbo) glGenBuffers(1, &m->vbo);
        glBindBuffer(GL_ARRAY_BUFFER, m->vbo);
        glBufferData(GL_ARRAY_BUFFER, m->vertex_count * sizeof_vertex, vertex_data, flags & MESH2_STREAM ? GL_STREAM_DRAW : GL_STATIC_DRAW);
    }

    // vertex setup: iterate descriptors
    for( int i = 0; i < countof(descriptor); ++i ) {
        if( descriptor[i].num_components ) {
            glVertexAttribPointer(i,
                descriptor[i].num_components, descriptor[i].vertex_type, descriptor[i].vertex_type == GL_UNSIGNED_BYTE ? GL_TRUE : GL_FALSE,
                sizeof_vertex, (GLchar*)NULL + descriptor[i].offset);
            glEnableVertexAttribArray(i);
        } else {
            glDisableVertexAttribArray(i);
        }
    }

    glBindVertexArray(0);
}

#if 0
void material3_bind( material3 *m, unsigned program ) {
    // assert( is_any_shader_bound() ); 

    int diffuse = 0, normal = 0, specular = 0, colormask = 0;
    if( program ) 
    for(GLuint i = 0; i < m->texture_count; i++) {
        int number = 0;
        /**/ if(m->texture_usage[i] == 'd') number = ++diffuse;
        else if(m->texture_usage[i] == 'n') number = ++normal;
        else if(m->texture_usage[i] == 's') number = ++specular;
        else if(m->texture_usage[i] == 'c') number = ++colormask;

        int texture_id = m->texture_id[i]; // *map_find(textures, material->texname);
        if (texture_id) {
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, m->texture_id[i]); // *map_find(textures, m->texture_name);
            glUniform1f(glGetUniformLocation(program, va("texture_%c_%d", m->texture_usage[i], number)), i);
        } else {
            PANIC("undefined binding: material_id %d -> texture_id %d (%s)\n", m->material_id, texture_id, m->texname );
        }
    }
}
#endif

void mesh_render(mesh* m, unsigned program) {
    // assert( is_any_shader_bound() ); 

#if 0
    if( program && m->material_id ) {
        material *material = &materials[m->material_id];
        material3_bind(material, program);
    }

    if( r->material && model ) {
        material_sendmodel(r->material, model);
    }
#endif

    if(program) {
        glUseProgram(program);
    }

    //
    glBindVertexArray(m->vao);
    if( m->ibo ) { // with indices
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m->ibo); // <-- why intel?
        glDrawElements(m->flags & MESH2_TRIANGLE_STRIP ? GL_TRIANGLE_STRIP : GL_TRIANGLES, m->index_count, GL_UNSIGNED_INT, (char*)0);
    } else { // with vertices only
        glDrawArrays(m->flags & MESH2_TRIANGLE_STRIP ? GL_TRIANGLE_STRIP : GL_TRIANGLES, 0, m->vertex_count /* / 3 */);
    }
    glBindVertexArray(0);

/*
    if(program) {
        glUseProgram(0);
    }
*/
}

void mesh_destroy(mesh* m) {
/*
    if(m->material) material_destroy(m->material);
    for( int i = 0; i < sizeof(m->mesh.vbo) / sizeof(m->mesh.vbo[0]); ++i ) {
        glDeleteBuffers(1, &m->mesh.vbo[i]);
    }
*/
    glDeleteBuffers(1, &m->ibo);
    glDeleteBuffers(1, &m->vbo);
    glDeleteVertexArrays(1, &m->vao);

    mesh z = {0};
    *m = z;
}

void mesh_loadmem(mesh *self, const char *data, size_t length) {
    // .bob format (little-endian):
    // [bytes_vp:4][bytes_vt:4][bytes_vn:4][bytes_idx:4][P3f...][T2f...][N3f...][IDX32...]
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
    uint32_t *idx = (uint32_t*)MALLOC(bytes_id);
    uint32_t count_id = bytes_id / sizeof(0[idx]);
    for( int i = 0; i < count_id; i += 3 ) {
        idx[ i / 3 ] = ((uint32_t*)id)[ i ];
    }
    id = idx;
    bytes_id /= 3;
    // convert buffers to single interleaved buffer
    float *vtx = (float*)MALLOC( bytes_vp + bytes_vt + bytes_vn ), *ptr = vtx;
    int32 vcount = bytes_vp/(3*sizeof(float));
    for( int i = 0; i < vcount; ++i ) {
        *ptr++ = ((float*)vp)[ i*3 + 0 ];
        *ptr++ = ((float*)vp)[ i*3 + 1 ];
        *ptr++ = ((float*)vp)[ i*3 + 2 ];

        *ptr++ = ((float*)vt)[ i*2 + 0 ];
        *ptr++ = ((float*)vt)[ i*2 + 1 ];

        *ptr++ = ((float*)vn)[ i*3 + 0 ];
        *ptr++ = ((float*)vn)[ i*3 + 1 ];
        *ptr++ = ((float*)vn)[ i*3 + 2 ];
    }
    mesh_create(self, "p3 t2 n3", 0,vcount,vtx, bytes_id/sizeof(uint32_t),idx, 0);

    FREE(vtx);
    FREE(idx);
}

void mesh_loadfile(mesh *self, const char *pathfile) {
    mesh_loadmem(self, file_read(pathfile), file_size(pathfile));
}

void mesh_make_quad( mesh *m ) {
    // A--B ; quad CDAB becomes triangle CAB and CBD.
    // | /| 
    // |/ | 
    // C--D 
    struct vertex_p3_c4b_t2 {
        vec3 p;
        uint32_t c;
        vec2 t;
    } stream[] = {
        /*C*/ { vec3(-1, -1, 0), 0xFF0000FF, vec2(0, 1) },
        /*A*/ { vec3(-1,  1, 0), 0xFF00FF00, vec2(0, 0) },
        /*B*/ { vec3( 1,  1, 0), 0xFFFF0000, vec2(1, 0) },
        /*C*/ { vec3(-1, -1, 0), 0xFF0000FF, vec2(0, 1) },
        /*B*/ { vec3( 1,  1, 0), 0xFFFF0000, vec2(1, 0) },
        /*D*/ { vec3( 1, -1, 0), 0xFFFFFFFF, vec2(1, 1) },
    };

    mesh_create( m, "p3 c4b t2", 0,6,stream, 0,NULL, 0);
}

#endif


