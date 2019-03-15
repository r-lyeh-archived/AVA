// original linmath.h header by @datenwolf (WTFPL2 licensed)

#ifndef LINEAR_H
#define LINEAR_H

// change #1
#include "../detect/detect_inline.c"

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
static m_inline void mat4_from_vec3_mul_outer(mat44 M, vec3 a, vec3 b)
{
    int i, j;
    for(i=0; i<4; ++i) for(j=0; j<4; ++j)
        M[i*4+j] = i<3 && j<3 ? a[i] * b[j] : 0.f;
}
static m_inline void mat4_rotate2(mat44 R, mat44 M, float x, float y, float z, float angle)
{
    float s = sinf(angle);
    float c = cosf(angle);
    vec3 u = {x, y, z};

    if( len3(u) > 1e-4 ) {
        u = norm3(u);
        mat44 T;
        mat4_from_vec3_mul_outer(T, u, u);

        mat44 S = {
            {    0,  u[2], -u[1], 0},
            {-u[2],     0,  u[0], 0},
            { u[1], -u[0],     0, 0},
            {    0,     0,     0, 0}
        };
        mat4_scale(S, S, s);

        mat44 C;
        identity4(C);
        mat4_sub(C, C, T);

        mat4_scale(C, C, c);

        mat4_add(T, T, C);
        mat4_add(T, T, S);

        T[3*4+3] = 1;
        multiply4(R, M, T);
    } else {
        dup4(R, M);
    }
}
#endif

// from wikipedia (in radians)
static
void toQuaternion(quat q, float pitch, float roll, float yaw) {
    // Abbreviations for the various angular functions
    float cy = cosf(yaw * 0.5);
    float sy = sinf(yaw * 0.5);
    float cr = cosf(roll * 0.5);
    float sr = sinf(roll * 0.5);
    float cp = cosf(pitch * 0.5);
    float sp = sinf(pitch * 0.5);

    q.x = cy * cr * cp + sy * sr * sp;
    q.y = cy * sr * cp - sy * cr * sp;
    q.z = cy * cr * sp + sy * sr * cp;
    q.w = sy * cr * cp - cy * sr * sp;
}
static
void toEulerAngle(const quat q, float *pitch, float *roll, float *yaw) {
    // roll (x-axis rotation)
    float sinr_cosp = +2.0 * (q.x * q.y + q.z * q.w);
    float cosr_cosp = +1.0 - 2.0 * (q.y * q.y + q.z * q.z);
    *roll = atan2f(sinr_cosp, cosr_cosp);

    // pitch (y-axis rotation)
    float sinp = +2.0 * (q.x * q.z - q.w * q.y);
    if (fabs(sinp) >= 1)
        *pitch = copysignf(M_PI / 2, sinp); // use 90 degrees if out of range
    else
        *pitch = asinf(sinp);

    // yaw (z-axis rotation)
    float siny_cosp = +2.0 * (q.x * q.w + q.y * q.z);
    float cosy_cosp = +1.0 - 2.0 * (q.z * q.z + q.w * q.w);
    *yaw = atan2f(siny_cosp, cosy_cosp);
}
static
void euler_to_quat(quat q, float pitch, float yaw, float roll) {
    pitch *= 0.0174533f; // deg to rad
    yaw *= 0.0174533f;   // deg to rad
    roll *= 0.0174533f;  // deg to rad

    float cr = (float)cos(-roll * 0.5);
    float sr = (float)sin(-roll * 0.5);
    float cp = (float)cos(-pitch * 0.5);
    float sp = (float)sin(-pitch * 0.5);
    float cy = (float)cos(yaw * 0.5);
    float sy = (float)sin(yaw * 0.5);

    q.x = cr * sp * cy - sr * cp * sy;
    q.y = cr * cp * sy + sr * sp * cy;
    q.z = sr * cp * cy - cr * sp * sy;
    q.w = cr * cp * cy + sr * sp * sy;
}


static m_inline void mat4_from_quat(mat44 M, quat q) {
    float a = q.w;
    float b = q.x;
    float c = q.y;
    float d = q.z;
    float a2 = a*a;
    float b2 = b*b;
    float c2 = c*c;
    float d2 = d*d;
    
    M[0*4+0] = a2 + b2 - c2 - d2;
    M[0*4+1] = 2.f*(b*c + a*d);
    M[0*4+2] = 2.f*(b*d - a*c);
    M[0*4+3] = 0.f;

    M[1*4+0] = 2*(b*c - a*d);
    M[1*4+1] = a2 - b2 + c2 - d2;
    M[1*4+2] = 2.f*(c*d + a*b);
    M[1*4+3] = 0.f;

    M[2*4+0] = 2.f*(b*d + a*c);
    M[2*4+1] = 2.f*(c*d - a*b);
    M[2*4+2] = a2 - b2 - c2 + d2;
    M[2*4+3] = 0.f;

    M[3*4+0] = M[3*4+1] = M[3*4+2] = 0.f;
    M[3*4+3] = 1.f;
}


#if 0

// out is vec4
void CreatePerspectiveProjection(float * proj, float aspect, float fov_y_rad, float n, float f) {
    float d = 1.f / (float)tan(fov_y_rad * 0.5f);
    proj[0] = d / aspect;
    proj[1] = d;
    proj[2] = f / (n - f);
    proj[3] = (f * n) / (n - f);
}

#endif


#if 0
// laurent couvidou
void rotate_vector_by_quat(vec3 vprime, const vec3 v, const quat q )
{
    // Extract the vector part of the quaternion
    vec3 u = {q[0], q[1], q[2]};

    // Extract the scalar part of the quaternion
    float s = q[3];

    // Do the math
    /*
    vprime = 2.0f * dot(u, v) * u
          + (s*s - dot(u, u)) * v
          + 2.0f * s * cross(u, v);
    */

    float ss = s * s;
    float s2 = s * 2;

    float duv2 = 2 * vec3_dot(u,v);
    float duuss = ss - vec3_dot(u,u);
    vec3 cuv; vec3_mul_cross(cuv, u, v);

    vprime[0] = duv2 * u[0] + duuss * v[0] + s2 * cuv[0];
    vprime[1] = duv2 * u[1] + duuss * v[1] + s2 * cuv[1];
    vprime[2] = duv2 * u[2] + duuss * v[2] + s2 * cuv[2];
}
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
typedef float quat[4];
static m_inline void quat_identity(quat q)
{
    q[0] = q[1] = q[2] = 0.f;
    q[3] = 1.f;
}
static m_inline void quat_add(quat r, quat a, quat b)
{
    int i;
    for(i=0; i<4; ++i)
        r[i] = a[i] + b[i];
}
static m_inline void quat_sub(quat r, quat a, quat b)
{
    int i;
    for(i=0; i<4; ++i)
        r[i] = a[i] - b[i];
}
static m_inline void quat_mul(quat r, quat p, quat q)
{
    vec3 w;
    r = cross3(p, q);
    vec3_scale(w, p, q[3]);
    vec3_add(r, r, w);
    vec3_scale(w, q, p[3]);
    vec3_add(r, r, w);
    r[3] = p[3]*q[3] - vec3_mul_inner(p, q);
}
static m_inline void quat_scale(quat r, quat v, float s)
{
    int i;
    for(i=0; i<4; ++i)
        r[i] = v[i] * s;
}
static m_inline float quat_inner_product(quat a, quat b)
{
    float p = 0.f;
    int i;
    for(i=0; i<4; ++i)
        p += b[i]*a[i];
    return p;
}
static m_inline void quat_conj(quat r, quat q)
{
    int i;
    for(i=0; i<3; ++i)
        r[i] = -q[i];
    r[3] = q[3];
}
static m_inline void quat_rotate(quat r, float angle, vec3 axis) {
    vec3 v;
    vec3_scale(v, axis, sinf(angle / 2));
    int i;
    for(i=0; i<3; ++i)
        r[i] = v[i];
    r[3] = cosf(angle / 2);
}
#define quat_norm vec4_norm
static m_inline void quat_mul_vec3(vec3 r, quat q, vec3 v)
{
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

static m_inline void mat4o_mul_quat(mat44 R, mat44 M, quat q)
{
/*  XXX: The way this is written only works for othogonal matrices. */
/* TODO: Take care of non-orthogonal case. */
    quat_mul_vec3(R[0], q, M[0]);
    quat_mul_vec3(R[1], q, M[1]);
    quat_mul_vec3(R[2], q, M[2]);

    R[3*4+0] = R[3*4+1] = R[3*4+2] = 0.f;
    R[3*4+3] = 1.f;
}
static m_inline void quat_from_mat4(quat q, mat44 M)
{
    float r=0.f;
    int i;

    int perm[] = { 0, 1, 2, 0, 1 };
    int *p = perm;

    for(i = 0; i<3; i++) {
        float m = M[i*4+i];
        if( m < r )
            continue;
        m = r;
        p = &perm[i];
    }

    r = sqrtf(1.f + M[p[0]*4+p[0]] - M[p[1]*4+p[1]] - M[p[2]*4+p[2]] );

    if(r < 1e-6) {
        q[0] = 1.f;
        q[1] = q[2] = q[3] = 0.f;
        return;
    }

    q[0] = r/2.f;
    q[1] = (M[p[0]*4+p[1]] - M[p[1]*4+p[0]])/(2.f*r);
    q[2] = (M[p[2]*4+p[0]] - M[p[0]*4+p[2]])/(2.f*r);
    q[3] = (M[p[2]*4+p[1]] - M[p[1]*4+p[2]])/(2.f*r);
}
#endif

#endif
