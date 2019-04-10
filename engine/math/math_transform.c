#ifndef TRANSFORM_H
#define TRANSFORM_H
#include "math_linear.c"

API float* transform( mat44 r, float position[3], float rotquat[4], float scale[3] );

#endif

#ifdef TRANSFORM_C
#pragma once

float* transform( mat44 r, float position[3], float rotquat[4], float scale[3] ) {
/*
    // commented out because of m_math.h
    mat44_from_quat(r, rotquat);
*/
    scaling44(r, scale[0], scale[1], scale[2]);
    translate44(r, position[0], position[1], position[2]);
    return r;
}

/*
static m_inline
float* transform( mat44 m, vec3 position, quat rotation, vec3 scaling) {
    mat44 r;
    mat44_from_quat(r, rotation);

    m[0][0] = r[0][0] * scaling.x;
    m[1][0] = r[1][0] * scaling.x;
    m[2][0] = r[2][0] * scaling.x;
    m[3][0] = position.x;

    m[0][1] = r[0][1] * scaling.y;
    m[1][1] = r[1][1] * scaling.y;
    m[2][1] = r[2][1] * scaling.y;
    m[3][1] = position.y;

    m[0][2] = r[0][2] * scaling.z;
    m[1][2] = r[1][2] * scaling.z;
    m[2][2] = r[2][2] * scaling.z;
    m[3][2] = position.z;

    m[0][3] = 0.0;
    m[1][3] = 0.0;
    m[2][3] = 0.0;
    m[3][3] = 1.0;

    return &m[0][0];
}
*/

#endif
