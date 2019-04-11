#ifndef MESH2_H
#define MESH2_H

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
};

typedef struct material3 {
    GLuint texture_id[8], texture_usage[8], texture_count;
} material3;

#pragma pack(1)
typedef struct mesh2 {
    GLuint vao, vbo, ibo;
    GLuint vertex_count, index_count;
    union {
    GLuint texture_id[1];
    GLuint material_id;
    };
    vec3 minbb, maxbb; // center+extents instead ?
    char name[64 - sizeof(GLuint) * 6 - sizeof(vec3) * 2]; // char *name; instead?
} mesh2;
#pragma pack()


API void   mesh2_create(mesh2* m, const char *format, int vertex_count, void *vertex_data, int index_count, void *index_data, int flags);
API void   mesh2_render(mesh2* m, unsigned program); // , int instanceCount = 1, GLenum mode /*GL_TRIANGLES*/);
API void   mesh2_destroy(mesh2* m);

#endif


#ifdef MESH2_C
#pragma once

#include <stdlib.h>
#include <stdarg.h>

typedef int mesh2_static_assert[ sizeof(mesh2) == 64 ];

void mesh2_create(mesh2* m, const char *format, int vertex_count, void *vertex_data, int index_count, void *index_data, int flags) {
    // assert( is_any_shader_bound() ); 

    mesh2 z = {0};
    *m = z;

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
        break; case 'b': if(format[-1] >= '0' && format[-1] <= '9') dc->vertex_type = GL_BYTE; //else bitangent.
        break; case ' ': case '\0': 
            if (!dc->vertex_type) dc->vertex_type = GL_FLOAT;
            dc->offset = sizeof_vertex;
            sizeof_vertex += (dc->stride = dc->num_components * (dc->vertex_type == GL_BYTE ? 1 : 4));
            ++dc;
        break; default: if( !strchr("pntcwai", *format) ) PANIC("unsupported vertex type '%c'", *format);
    } while (*format++);

    // layout
    glGenVertexArrays(1, &m->vao);
    glBindVertexArray(m->vao);

    // index data
    if( index_data && index_count ) {
        m->index_count = index_count;

        glGenBuffers(1, &m->ibo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m->ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, m->index_count * sizeof_index, index_data, GL_STATIC_DRAW);
    }

    // vertex data
    if( vertex_data && vertex_count ) {
        m->vertex_count = vertex_count;

        glGenBuffers(1, &m->vbo);
        glBindBuffer(GL_ARRAY_BUFFER, m->vbo);
        glBufferData(GL_ARRAY_BUFFER, m->vertex_count * sizeof_vertex, vertex_data, GL_STATIC_DRAW);
    }

    // vertex setup: iterate descriptors
    for( int i = 0; i < countof(descriptor); ++i ) {
        if( descriptor[i].num_components ) {
            glVertexAttribPointer(i, descriptor[i].num_components, descriptor[i].vertex_type, GL_FALSE, sizeof_vertex, (GLchar*)NULL + descriptor[i].offset );
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

void mesh2_render(mesh2* m, unsigned program) {
    // assert( is_any_shader_bound() ); 

#if 0
    if( program && m->material_id ) {
        material *material = &materials[m->material_id];
        material3_bind(material, program);
    }
#endif

    // bind vao
    glBindVertexArray(m->vao);

    // draw mesh
    if( m->ibo ) {
        // with indices
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m->ibo); // <-- why? intel only?
        glDrawElements(GL_TRIANGLES, m->index_count, GL_UNSIGNED_INT, (char*)0);
    } else {
        // without indices
        glDrawArrays(GL_TRIANGLES, 0, m->vertex_count /* / 3 */);
    }

    // unbind buffers
    glBindVertexArray(0);
}

void mesh2_destroy(mesh2* m) {
    glDeleteBuffers(1, &m->ibo);
    glDeleteBuffers(1, &m->vbo);
    glDeleteVertexArrays(1, &m->vao);
}

/*
mesh2 *mesh2_parse_bob(mesh2 *self, const char *data, size_t length) {
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
    uint32_t *id2 = (uint32_t*)malloc(bytes_id);
    uint32_t count_id = bytes_id / sizeof(0[id2]);
    for( int i = 0; i < count_id; i += 3 ) {
        id2[ i / 3 ] = ((uint32_t*)id)[ i ];
    }
    id = id2;
    bytes_id /= 3;

    // convert vertex buffers to interleaved buffer
    float *vtx2 = (float*)malloc( bytes_vp + bytes_vt + bytes_vn );
    int32 vcount = bytes_vp/(3*4)/3;
    for( int i = 0; i < vcount; ++i ) {
        *vtx2++ = ((float*)vp)[ i*3 + 0 ];
        *vtx2++ = ((float*)vp)[ i*3 + 1 ];
        *vtx2++ = ((float*)vp)[ i*3 + 2 ];

        *vtx2++ = ((float*)vt)[ i*2 + 0 ];
        *vtx2++ = ((float*)vt)[ i*2 + 1 ];

        *vtx2++ = ((float*)vn)[ i*3 + 0 ];
        *vtx2++ = ((float*)vn)[ i*3 + 1 ];
        *vtx2++ = ((float*)vn)[ i*3 + 2 ];
    }
    mesh2_create(self, "p3 t2 n3", vcount, vtx2, bytes_id / sizeof(uint32_t), id2, 0);
    
    free(vtx2);
    free(id2);
    return self;
}

mesh2 *mesh2_load_bob(mesh2 *self, const char *pathfile) {
    return mesh2_parse_bob(self, file_read_(pathfile), file_size_(pathfile)); // LEAK
}
*/

#endif
