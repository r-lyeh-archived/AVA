// original linmath.h header by @datenwolf (WTFPL2 licensed)

#ifndef LINEAR_H
#define LINEAR_H

#include "core/detect/detect_inline.c"

#include "math_linear2.c"


#if 0
static m_inline void mat4_add(mat44 M, mat44 a, mat44 b) {
    int i;
    for(i=0; i<4; ++i)
        vec4_add(M[i], a[i], b[i]);
}
static m_inline void mat4_sub(mat44 M, mat44 a, mat44 b) {
    int i;
    for(i=0; i<4; ++i)
        vec4_sub(M[i], a[i], b[i]);
}
static m_inline void mat4_scale(mat44 M, mat44 a, float k) {
    int i;
    for(i=0; i<4; ++i)
        vec4_scale(M[i], a[i], k);
}
#endif




#if 0
void extract_direction( vec3 forward, vec3 right, vec3 up, const quat q ) {
    vec3 u = {0,1,0};
    vec3 f = {0,0,-1};
    vec3 r = {1,0,0};
    rotate_vector_by_quat( up, u, q );
    rotate_vector_by_quat( forward, f, q );
    rotate_vector_by_quat( right, r, q );
}

void GetRotation(float m[16], float& Yaw, float& Pitch, float& Roll) {
    // @test me!
    //     [0] [4] [8 ] [12]
    //     [1] [5] [9 ] [13]
    //     [2] [6] [10] [14]
    //     [3] [7] [11] [15]

    if( m[0] == 1.0f ) {
        Yaw = atan2f( m[2], m[11] );
        Pitch = 0;
        Roll = 0;
    }
    else if( m[0] == -1.0f ) {
        Yaw = atan2f( m[2], m[11] );
        Pitch = 0;
        Roll = 0;
    }
    else {
        Yaw = atan2( -m[8], m[0] );
        Pitch = asin( m[4] );
        Roll = atan2( -m[6], m[5] );
    }
}

#endif

#if 0
static m_inline void vec3_reflect(vec3 r, vec3 const v, vec3 const n) {
    float p = 2.f * dot3(v, n);
    for(int i=0;i<3;++i)
        r[i] = v[i] - p*n[i];
}

static m_inline void vec4_reflect(vec4 r, vec4 v, vec4 n) {
    float p  = 2.f * dot4(v, n);
    for(int i=0;i<4;++i)
        r[i] = v[i] - p*n[i];
}
#endif

#if 0
static m_inline void mul_vec3q(vec3 r, quat q, vec3 v) {
/*
 * Method by Fabian 'ryg' Giessen (of Farbrausch)
t = 2 * cross(q.xyz, v)
v' = v + q.w * t + cross(q.xyz, t)
 */
    vec3 t;
    vec3 q_xyz = {q[0], q[1], q[2]};
    vec3 u = {q[0], q[1], q[2]};

    t = cross3(q_xyz, v);
    vec3_scale(t, t, 2);

    u = cross3(q_xyz, t);
    vec3_scale(t, t, q[3]);

    vec3_add(r, v, t);
    vec3_add(r, r, u);
}
#endif


#endif
