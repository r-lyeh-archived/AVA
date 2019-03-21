#ifndef MESH2_H
#define MESH2_H

#include "render/render_opengl.c"
#include "math.h"

#ifndef offsetof
#error
#endif

typedef struct mesh2 {
    GLuint vao, vbo, ibo, texture[8];
    GLuint vcount, icount, stride;
    mat44 transform;
} mesh2;

void   mesh2_create(mesh2*, GLfloat* vertices, size_t vcount, GLuint* indices, size_t icount, const char *format, unsigned stride, ...);
void   mesh2_draw(mesh2*);
void   mesh2_destroy(mesh2* m);

#endif


#ifdef MESH2_C
#pragma once

#include <stdlib.h>
#include <stdarg.h>

void mesh2_create(mesh2 *m, GLfloat* vertices, size_t vcount, GLuint* indices, size_t icount, const char *format, unsigned stride, ...) {
    mesh2 c = {0};
    *m = c;

    m->stride = stride;
    identity44(m->transform);

    // layout
    glGenVertexArrays(1, &m->vao);
    glBindVertexArray(m->vao);

    // index
    if( indices && icount ) {
        m->icount = icount;

        glGenBuffers(1, &m->ibo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m->ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, m->icount * sizeof(GLuint), indices, GL_STATIC_DRAW);
    }

    // vertex
    if( vertices && vcount ) {
        m->vcount = vcount;

        glGenBuffers(1, &m->vbo);
        glBindBuffer(GL_ARRAY_BUFFER, m->vbo);
        glBufferData(GL_ARRAY_BUFFER, m->vcount * m->stride, vertices, GL_STATIC_DRAW);
    }

    // configure vertex stream
    va_list lst;
    va_start(lst, stride);
    int attrib = 0;
    for( const char *c = format; *c ; c++, attrib++ ) {
        GLenum vertex_type = GL_FLOAT; // unsupported: GL_BOOL,GL_DOUBLE,GL_INT,GL_BYTE,GL_SHORT,GL_UNSIGNED_INT,GL_UNSIGNED_SHORT,
        GLenum vertex_normalized = GL_FALSE;
        int offset = va_arg(lst, int);
        int num_components = *c++ - '0';
        /**/ if( c[0] == 'f' || c[0] == 'F' ) { c++; vertex_type = GL_FLOAT; }
        else if( c[0] == 'u' || c[0] == 'U' ) { c++; vertex_type = GL_UNSIGNED_BYTE; }
        if( num_components ) {
            glVertexAttribPointer(attrib, num_components, vertex_type, vertex_normalized, stride, (GLvoid*)((char*)NULL + offset));
            glEnableVertexAttribArray(attrib);
        } else {
            glDisableVertexAttribArray(attrib);
        }
    }
    va_end(lst);

#if 1
    glBindVertexArray(0);
#endif
}

void mesh2_draw(mesh2* m) {
    // bind vao
    glBindVertexArray(m->vao);

    // draw mesh
    if( m->ibo ) {
        // with indices
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m->ibo);
        glDrawElements(GL_TRIANGLES, m->icount, GL_UNSIGNED_INT, (char*)0);
    } else {
        // without indices
        glDrawArrays(GL_TRIANGLES, 0, m->vcount / 3);
    }

#if 1
    // unbind buffers
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
#endif
}

void mesh2_destroy(mesh2* m) {
    glDeleteBuffers(1, &m->ibo);
    glDeleteBuffers(1, &m->vbo);
    glDeleteVertexArrays(1, &m->vao);
}

#endif
