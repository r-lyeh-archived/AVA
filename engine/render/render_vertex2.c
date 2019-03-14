#ifndef VERTEX2_H
#define VERTEX2_H

API unsigned vertex_make();

#endif

#ifdef VERTEX2_C
#pragma once
#include "render_opengl.c"

unsigned vertex_make() {
    GLuint vertex_array_object;
    glGenVertexArrays(1, &vertex_array_object);
    glBindVertexArray(vertex_array_object);
    return vertex_array_object;
}

#endif
