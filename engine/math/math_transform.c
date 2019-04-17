#ifndef TRANSFORM_H
#define TRANSFORM_H
#include "math_linear.c"

// ----------------------------------------------------------------------------

static m_inline vec3 transform33(mat33 m, vec3 p) {
    float x = (m[0] * p.x) + (m[4] * p.y) + (m[ 8] * p.z);
    float y = (m[1] * p.x) + (m[5] * p.y) + (m[ 9] * p.z);
    float z = (m[2] * p.x) + (m[6] * p.y) + (m[10] * p.z);
    return vec3(x,y,z);
}

static m_inline vec4 transform444(const mat44 m, const vec4 p) {
    // remember w = 1 for move in space; w = 0 rotate in space;
    float x = m[0]*p.x + m[4]*p.y + m[ 8]*p.z + m[12]*p.w;
    float y = m[1]*p.x + m[5]*p.y + m[ 9]*p.z + m[13]*p.w;
    float z = m[2]*p.x + m[6]*p.y + m[10]*p.z + m[14]*p.w;
    float w = m[3]*p.x + m[7]*p.y + m[11]*p.z + m[15]*p.w;
    return vec4(x,y,z,w);
}

static m_inline vec3 transform344(const mat44 m, const vec3 p) {
    vec4 v = transform444(m, vec34(p, 1));
    return scale3(v.vec3, 1.f / v.w);
}

static m_inline vec3 transformq(const quat q, const vec3 v) {  // !!! ok, i guess
    // [src] https://gamedev.stackexchange.com/questions/28395/rotating-vector3-by-a-quaternion (laurent couvidou)
    // Extract the vector part of the quaternion
    vec3 u = vec3(q.x, q.y, q.z);
    // Extract the scalar part of the quaternion
    float s = q.w;
    // Do the math
    vec3 a = scale3(u, 2 * dot3(u,v));
    vec3 b = scale3(v, s*s - dot3(u,u));
    vec3 c = scale3(cross3(u,v), 2*s);
    return add3(a, add3(b,c));
}

static m_inline vec3 transform_axis(const coord_system basis, const coord_axis to) { 
    const float dot_table[6][6] = {
        {+1,-1,0,0,0,0},{-1,+1,0,0,0,0},{0,0,+1,-1,0,0},
        {0,0,-1,+1,0,0},{0,0,0,0,+1,-1},{0,0,0,0,-1,+1},
    };
    return vec3( dot_table[basis[0]][to], dot_table[basis[1]][to], dot_table[basis[2]][to] );
}

// A vector is the difference between two points in 3D space, possessing both direction and magnitude
static m_inline vec3 transform_vector  (const mat44 m, const vec3 vector)   {
    return transform344(m, vector);
}

// A point is a specific location within a 3D space
static m_inline vec3 transform_point   (const mat44 m, const vec3 p)    { // return (m * vec4{point,1).xyz()/r.w;
    float inv = 1.0f / (m[3+4*0]*p.x + m[3+4*1]*p.y + m[3+4*2]*p.z + m[3+4*3]);
    return vec3(
        (m[0+4*0]*p.x + m[0+4*1]*p.y + m[0+4*2]*p.z + m[0+4*3]) * inv,
        (m[1+4*0]*p.x + m[1+4*1]*p.y + m[1+4*2]*p.z + m[1+4*3]) * inv,
        (m[2+4*0]*p.x + m[2+4*1]*p.y + m[2+4*2]*p.z + m[2+4*3]) * inv
    );
}

// A tangent is a unit-length vector which is parallel to a piece of geometry, such as a surface or a curve
static m_inline vec3 transform_tangent (const mat44 m, const vec3 tangent)  { return norm3(transform_vector(m, tangent)); }

// A normal is a unit-length bivector which is perpendicular to a piece of geometry, such as a surface or a curve
static m_inline vec3 transform_normal  (const mat44 m, const vec3 normal)   {
    mat44 t; transpose44(t,m); mat44 i; invert44(i,t);
    return scale3(norm3(transform_vector(i, normal)), det44(m) < 0 ? -1 : 1);
}

// A quaternion can describe both a rotation and a uniform scaling in 3D space
static m_inline quat transform_quat     (const mat44 m, const quat q)      {
    vec3 s = scale3(transform_vector(m, q.vec3), det44(m) < 0 ? -1 : 1);
    return quat(s.x,s.y,s.z,q.w);
}

// A matrix can describe a general transformation of homogeneous coordinates in projective space
static m_inline float* transform_matrix(mat44 out, const mat44 m, const mat44 matrix) {
    mat44 I; invert44(I, m);
    mat44 N; multiply344(out, I, matrix, m); // m,matrix,I instead ?
    return out;
}

// Scaling factors are not a vector, they are a compact representation of a scaling matrix
static m_inline vec3 transform_scaling (const mat44 m, const vec3 scaling) {
    mat44 s; scaling44(s, scaling.x, scaling.y, scaling.z);
    mat44 out; transform_matrix(out, m, s);
    return vec3( out[0], out[5], out[10] );
}

#endif

