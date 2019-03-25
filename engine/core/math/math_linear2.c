// Original authors: @ands (Andreas Mantler; Public Domain) + @datenwolf (WTFPL2 licensed)
// - rlyeh, public domain.

#pragma once
#include <math.h>
#include <float.h>

#define M__PI 3.14159265358979323846f

#if defined(_MSC_VER) && !defined(__cplusplus)
#define m_inline __inline
#else
#define m_inline inline
#endif

#if defined(_MSC_VER) && (_MSC_VER <= 1700)
#define m_finite _finite
#else
#define m_finite isfinite
#endif

#ifdef __cplusplus
#define M_CAST(type, ...)  ( type { __VA_ARGS__ } )
#else
#define M_CAST(type, ...)  ((type){ __VA_ARGS__ } )
#endif

#define vec2(x, y      ) M_CAST(vec2, (x), (y)          )
#define vec3(x, y, z   ) M_CAST(vec3, (x), (y), (z),    )
#define vec4(x, y, z, w) M_CAST(vec4, (x), (y), (z), (w))
#define quat(x, y, z, w) M_CAST(quat, (x), (y), (z), (w))
#define mat44(...)       M_CAST(mat44, __VA_ARGS__ )

typedef union vec2 { struct { float x, y; }; struct { float r, g; }; float v[1]; } vec2;
typedef union vec3 { struct { float x, y, z; }; struct { float r, g, b; }; float v[1]; vec2 v2; } vec3;
typedef union vec4 { struct { float x, y, z, w; }; struct { float r, g, b, a; }; float v[1]; vec2 v2; vec3 v3; } vec4;
typedef union quat { struct { float x, y, z, w; }; float v[1]; vec3 v3; vec4 v4; } quat;

typedef float mat44[16];

static m_inline float deg      (float radians)      { return radians / M__PI * 180.0f; }
static m_inline float rad      (float degrees)      { return degrees * M__PI / 180.0f; }

static m_inline int   mini     (int    a, int    b) { return a < b ? a : b; }
static m_inline int   maxi     (int    a, int    b) { return a > b ? a : b; }
static m_inline int   absi     (int    a          ) { return a < 0 ? -a : a; }
static m_inline float minf     (float  a, float  b) { return a < b ? a : b; }
static m_inline float maxf     (float  a, float  b) { return a > b ? a : b; }
static m_inline float absf     (float  a          ) { return a < 0.0f ? -a : a; }
static m_inline float pmodf    (float  a, float  b) { return (a < 0.0f ? 1.0f : 0.0f) + (float)fmod(a, b); } // positive mod

static m_inline vec2  neg2     (vec2   a          ) { return vec2(-a.x, -a.y); }
static m_inline vec2  add2     (vec2   a, vec2   b) { return vec2(a.x + b.x, a.y + b.y); }
static m_inline vec2  sub2     (vec2   a, vec2   b) { return vec2(a.x - b.x, a.y - b.y); }
static m_inline vec2  mul2     (vec2   a, vec2   b) { return vec2(a.x * b.x, a.y * b.y); }
static m_inline vec2  scale2   (vec2   a, float  b) { return vec2(a.x * b, a.y * b); }
static m_inline vec2  div2     (vec2   a, float  b) { return scale2(a, 1.0f / b); }
static m_inline vec2  pmod2    (vec2   a, float  b) { return vec2(pmodf(a.x, b), pmodf(a.y, b)); }
static m_inline vec2  min2     (vec2   a, vec2   b) { return vec2(minf(a.x, b.x), minf(a.y, b.y)); }
static m_inline vec2  max2     (vec2   a, vec2   b) { return vec2(maxf(a.x, b.x), maxf(a.y, b.y)); }
static m_inline vec2  abs2     (vec2   a          ) { return vec2(absf(a.x), absf(a.y)); }
static m_inline vec2  floor2   (vec2   a          ) { return vec2(floorf(a.x), floorf(a.y)); }
static m_inline vec2  ceil2    (vec2   a          ) { return vec2(ceilf (a.x), ceilf (a.y)); }
static m_inline float dot2     (vec2   a, vec2   b) { return a.x * b.x + a.y * b.y; }
static m_inline float cross2   (vec2   a, vec2   b) { return a.x * b.y - a.y * b.x; } // pseudo cross product
static m_inline float len2sq   (vec2   a          ) { return a.x * a.x + a.y * a.y; }
static m_inline float len2     (vec2   a          ) { return sqrtf(len2sq(a)); }
static m_inline vec2  norm2    (vec2   a          ) { return div2(a, len2(a)); }
static m_inline int   finite2  (vec2   a          ) { return m_finite(a.x) && m_finite(a.y); }

static m_inline vec3  neg3     (vec3   a          ) { return vec3(-a.x, -a.y, -a.z); }
static m_inline vec3  add3     (vec3   a, vec3   b) { return vec3(a.x + b.x, a.y + b.y, a.z + b.z); }
static m_inline vec3  sub3     (vec3   a, vec3   b) { return vec3(a.x - b.x, a.y - b.y, a.z - b.z); }
static m_inline vec3  mul3     (vec3   a, vec3   b) { return vec3(a.x * b.x, a.y * b.y, a.z * b.z); }
static m_inline vec3  scale3   (vec3   a, float  b) { return vec3(a.x * b, a.y * b, a.z * b); }
static m_inline vec3  div3     (vec3   a, float  b) { return scale3(a, 1.0f / b); }
static m_inline vec3  pmod3    (vec3   a, float  b) { return vec3(pmodf(a.x, b), pmodf(a.y, b), pmodf(a.z, b)); }
static m_inline vec3  min3     (vec3   a, vec3   b) { return vec3(minf(a.x, b.x), minf(a.y, b.y), minf(a.z, b.z)); }
static m_inline vec3  max3     (vec3   a, vec3   b) { return vec3(maxf(a.x, b.x), maxf(a.y, b.y), maxf(a.z, b.z)); }
static m_inline vec3  abs3     (vec3   a          ) { return vec3(absf(a.x), absf(a.y), absf(a.z)); }
static m_inline vec3  floor3   (vec3   a          ) { return vec3(floorf(a.x), floorf(a.y), floorf(a.z)); }
static m_inline vec3  ceil3    (vec3   a          ) { return vec3(ceilf (a.x), ceilf (a.y), ceilf (a.z)); }
static m_inline float dot3     (vec3   a, vec3   b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
static m_inline vec3  cross3   (vec3   a, vec3   b) { return vec3(a.y * b.z - b.y * a.z, a.z * b.x - b.z * a.x, a.x * b.y - b.x * a.y); }
static m_inline float len3sq   (vec3   a          ) { return a.x * a.x + a.y * a.y + a.z * a.z; }
static m_inline float len3     (vec3   a          ) { return sqrtf(len3sq(a)); }
static m_inline vec3  norm3    (vec3   a          ) { return div3(a, len3(a)); }
static m_inline int   finite3  (vec3   a          ) { return m_finite(a.x) && m_finite(a.y) && m_finite(a.z); }
//static m_inline vec3 cross3tri (vec3 a, vec3 b, vec3 c) { return cross3(a,cross3(b,c)); }

static m_inline vec4  neg4     (vec4   a          ) { return vec4(-a.x, -a.y, -a.z, -a.w); }
static m_inline vec4  add4     (vec4   a, vec4   b) { return vec4(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w); }
static m_inline vec4  sub4     (vec4   a, vec4   b) { return vec4(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w); }
static m_inline vec4  mul4     (vec4   a, vec4   b) { return vec4(a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w); }
static m_inline vec4  scale4   (vec4   a, float  b) { return vec4(a.x * b, a.y * b, a.z * b, a.w * b); }
static m_inline vec4  div4     (vec4   a, float  b) { return scale4(a, 1.0f / b); }
static m_inline vec4  pmod4    (vec4   a, float  b) { return vec4(pmodf(a.x, b), pmodf(a.y, b), pmodf(a.z, b), pmodf(a.w, b)); }
static m_inline vec4  min4     (vec4   a, vec4   b) { return vec4(minf(a.x, b.x), minf(a.y, b.y), minf(a.z, b.z), minf(a.w, b.w)); }
static m_inline vec4  max4     (vec4   a, vec4   b) { return vec4(maxf(a.x, b.x), maxf(a.y, b.y), maxf(a.z, b.z), maxf(a.w, b.w)); }
static m_inline vec4  abs4     (vec4   a          ) { return vec4(absf(a.x), absf(a.y), absf(a.z), absf(a.w)); }
static m_inline vec4  floor4   (vec4   a          ) { return vec4(floorf(a.x), floorf(a.y), floorf(a.z), floorf(a.w)); }
static m_inline vec4  ceil4    (vec4   a          ) { return vec4(ceilf (a.x), ceilf (a.y), ceilf (a.z), ceilf (a.w)); }
static m_inline float dot4     (vec4   a, vec4   b) { return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w; }
static m_inline float len4sq   (vec4   a          ) { return a.x * a.x + a.y * a.y + a.z * a.z + a.w * a.w; }
static m_inline float len4     (vec4   a          ) { return sqrtf(len4sq(a)); }
static m_inline vec4  norm4    (vec4   a          ) { return div4(a, len4(a)); }
static m_inline int   finite4  (vec4   a          ) { return m_finite(a.x) && m_finite(a.y) && m_finite(a.z) && m_finite(a.w); }

static m_inline quat  identityq(                  ) { return quat(0,0,0,1); }
static m_inline quat  negq     (quat   a          ) { return quat(-a.x,-a.y,-a.z,a.w); } // conjq
static m_inline quat  addq     (quat   a, quat   b) { return quat(a.x+b.x,a.y+b.y,a.z+b.z,a.w+b.w); }
static m_inline quat  subq     (quat   a, quat   b) { return quat(a.x-b.x,a.y-b.y,a.z-b.z,a.w-b.w); }
static m_inline quat  scaleq   (quat   a, float  s) { return quat(a.x*s,a.y*s,a.z*s,a.w*s); }
static m_inline float dotq     (quat   a, quat   b) { return a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w; }
static m_inline quat  normq    (quat   a          ) { vec4 v = norm4(a.v4); return quat(v.x,v.y,v.z,v.w); }
static m_inline quat  mulq     (quat   p, quat   q) { vec3 w = scale3(p.v3, q.w), r = add3(add3(cross3(p.v3, q.v3), w), scale3(q.v3, p.w)); return quat(r.x,r.y,r.z,p.w*q.w - dot3(p.v3, q.v3)); }
static m_inline quat  eulerq   (float pitch_deg, float roll_deg, float yaw_deg) {
    float hp = rad(pitch_deg) * 0.5f, hr = rad(roll_deg) * 0.5f, hy = rad(yaw_deg) * 0.5f;
    float cy = cosf(hy), sy = sinf(hy), cr = cosf(hr), sr = sinf(hr), cp = cosf(hp), sp = sinf(hp);
    return quat(cy * cr * cp + sy * sr * sp, cy * sr * cp - sy * cr * sp, cy * cr * sp + sy * sr * cp, sy * cr * cp - cy * sr * sp);
}

static m_inline quat  rotationq(float deg,float x,float y,float z){ vec3 v = scale3(vec3(x,y,z), sinf(rad(deg)*0.5f)); return quat(v.x,v.y,v.z,cosf(rad(deg)*0.5f)); }
static m_inline quat  mat44q   (mat44 M) {
    float r=0.f;
    int perm[] = { 0, 1, 2, 0, 1 }, *p = perm;
    for(int i = 0; i<3; i++) {
        float m = M[i*4+i];
        if( m < r ) continue;
        m = r;
        p = &perm[i];
    }
    r = sqrtf(1.f + M[p[0]*4+p[0]] - M[p[1]*4+p[1]] - M[p[2]*4+p[2]] );
    return r >= 1e-6 ? quat(1,0,0,0)
    : quat(r/2.f, (M[p[0]*4+p[1]] - M[p[1]*4+p[0]])/(2.f*r), (M[p[2]*4+p[0]] - M[p[0]*4+p[2]])/(2.f*r), (M[p[2]*4+p[1]] - M[p[1]*4+p[2]])/(2.f*r) );
}



static m_inline void ortho44(float *m, float l, float r, float b, float t, float n, float f) {
    m[ 0] = 2/(r-l);      m[ 1] = 0;            m[ 2] = 0;            m[ 3] = 0;
    m[ 4] = 0;            m[ 5] = 2/(t-b);      m[ 6] = 0;            m[ 7] = 0;
    m[ 8] = 0;            m[ 9] = 0;            m[10] = -2/(f-n);     m[11] = 0;
    m[12] = -(r+l)/(r-l); m[13] = -(t+b)/(t-b); m[14] = -(f+n)/(f-n); m[15] = 1;
}
static m_inline void frustum44(float *m, float l, float r, float b, float t, float n, float f) {
    m[ 0] = 2*n/(r-l);   m[ 1] = 0;           m[ 2] = 0;               m[ 3] = 0;
    m[ 4] = 0;           m[ 5] = 2.*n/(t-b);  m[ 6] = 0;               m[ 7] = 0;
    m[ 8] = (r+l)/(r-l); m[ 9] = (t+b)/(t-b); m[10] = -(f+n)/(f-n);    m[11] = -1;
    m[12] = 0;           m[13] = 0;           m[14] = -2*(f*n)/(f-n);  m[15] = 0;
}
static m_inline void perspective44(float *m, float fovy_degrees, float aspect, float nearp, float farp) {
    float y = tanf(fovy_degrees * M__PI / 360) * nearp, x = y * aspect;
    frustum44(m, -x, x, -y, y, nearp, farp);
}
static m_inline void lookat44(float *m, vec3 eye, vec3 center, vec3 up) {
    vec3 f = norm3(sub3(center, eye));
    vec3 s = norm3(cross3(f, up));
    vec3 u = cross3(s, f);
    m[ 0] = s.x;           m[ 1] = u.x;           m[ 2] = -f.x;         m[ 3] = 0;
    m[ 4] = s.y;           m[ 5] = u.y;           m[ 6] = -f.y;         m[ 7] = 0;
    m[ 8] = s.z;           m[ 9] = u.z;           m[10] = -f.z;         m[11] = 0;
    m[12] = -dot3(s, eye); m[13] = -dot3(u, eye); m[14] = dot3(f, eye); m[15] = 1;
}

static m_inline void translation44(float *m, float x, float y, float z) { // identity4 + translate4
    m[ 0] = 1.0f; m[ 1] = 0.0f; m[ 2] = 0.0f; m[ 3] = 0.0f;
    m[ 4] = 0.0f; m[ 5] = 1.0f; m[ 6] = 0.0f; m[ 7] = 0.0f;
    m[ 8] = 0.0f; m[ 9] = 0.0f; m[10] = 1.0f; m[11] = 0.0f;
    m[12] = x;    m[13] = y;    m[14] = z;    m[15] = 1.0f;
}
static m_inline void translate44(float *m, float x, float y, float z) { // translate in place
    vec4 t = {x, y, z, 0};
    m[12] += dot4(vec4(m[0],m[4],m[ 8],m[12]),t); // row4(M,0)
    m[13] += dot4(vec4(m[1],m[5],m[ 9],m[13]),t); // row4(M,1)
    m[14] += dot4(vec4(m[2],m[6],m[10],m[14]),t); // row4(M,2)
    m[15] += dot4(vec4(m[3],m[7],m[11],m[15]),t); // row4(M,3)
}
static m_inline void rotation44(float *m, float degrees, float x, float y, float z) {
    if(len3sq(vec3(x,y,z)) < (1e-4 * 1e-4)) return;

    float radians = degrees * M__PI / 180.0f;
    float c = cosf(radians), s = sinf(radians), c1 = 1.0f - c;
    m[ 0] = x*x*c1 + c;   m[ 1] = y*x*c1 + z*s; m[ 2] = x*z*c1 - y*s; m[ 3] = 0.0f;
    m[ 4] = x*y*c1 - z*s; m[ 5] = y*y*c1 + c;   m[ 6] = y*z*c1 + x*s; m[ 7] = 0.0f;
    m[ 8] = x*z*c1 + y*s; m[ 9] = y*z*c1 - x*s; m[10] = z*z*c1 + c;   m[11] = 0.0f;
    m[12] = 0.0f;         m[13] = 0.0f;         m[14] = 0.0f;         m[15] = 1.0f;
}
static m_inline void rotate44(float *m, float degrees, float x, float y, float z) { // !!! ok, i guess
    if(len3sq(vec3(x,y,z)) < (1e-4 * 1e-4)) return;

    float c = cosf(rad(degrees)), s = -sinf(rad(degrees)), c1 = 1 - c;
    float m00 = m[ 0],  m01 = m[ 1],  m02 = m[ 2], m03 = m[ 3],
          m04 = m[ 4],  m05 = m[ 5],  m06 = m[ 6], m07 = m[ 7],
          m08 = m[ 8],  m09 = m[ 9],  m10 = m[10], m11 = m[11];

    // rotation matrix
    float r00 = x*x*c1 + c,   r01 = y*x*c1 + z*s, r02 = x*z*c1 - y*s;
    float r04 = x*y*c1 - z*s, r05 = y*y*c1 + c,   r06 = y*z*c1 + x*s;
    float r08 = x*z*c1 + y*s, r09 = y*z*c1 - x*s, r10 = z*z*c1 + c;

    // multiply
    m[ 0] = r00 * m00 + r04 * m04 + r08 * m08;
    m[ 1] = r00 * m01 + r04 * m05 + r08 * m09;
    m[ 2] = r00 * m02 + r04 * m06 + r08 * m10;
    m[ 3] = r00 * m03 + r04 * m07 + r08 * m11;
    m[ 4] = r01 * m00 + r05 * m04 + r09 * m08;
    m[ 5] = r01 * m01 + r05 * m05 + r09 * m09;
    m[ 6] = r01 * m02 + r05 * m06 + r09 * m10;
    m[ 7] = r01 * m03 + r05 * m07 + r09 * m11;
    m[ 8] = r02 * m00 + r06 * m04 + r10 * m08;
    m[ 9] = r02 * m01 + r06 * m05 + r10 * m09;
    m[10] = r02 * m02 + r06 * m06 + r10 * m10;
    m[11] = r02 * m03 + r06 * m07 + r10 * m11;
}
static m_inline void quat44(mat44 m, quat q) {
    float a  = q.w, b  = q.x, c  = q.y, d  = q.z;
    float a2 = a*a, b2 = b*b, c2 = c*c, d2 = d*d;
    m[ 0] = a2 + b2 - c2 - d2; m[ 1] = 2*(b*c + a*d);     m[ 2] = 2*(b*d - a*c);     m[ 3] = 0;
    m[ 4] = 2*(b*c - a*d);     m[ 5] = a2 - b2 + c2 - d2; m[ 6] = 2*(c*d + a*b);     m[ 7] = 0;
    m[ 8] = 2*(b*d + a*c);     m[ 9] = 2*(c*d - a*b);     m[10] = a2 - b2 - c2 + d2; m[11] = 0;
    m[12] = 0;                 m[13] = 0;                 m[14] = 0;                 m[15] = 1;
}
static m_inline void scaling44(float *m, float x, float y, float z) { // !!! ok, i guess
    m[ 0] = x; m[ 1] = 0; m[ 2] = 0; m[ 3] = 0;
    m[ 4] = 0; m[ 5] = y; m[ 6] = 0; m[ 7] = 0;
    m[ 8] = 0; m[ 9] = 0; m[10] = z; m[11] = 0;
    m[12] = 0; m[13] = 0; m[14] = 0; m[15] = 1;
}
static m_inline void scale44(float *m, float x, float y, float z) { // !!! ok, i guess
    m[ 0] *= x; m[ 1] *= x; m[ 2] *= x; m[ 3] *= x;
    m[ 4] *= y; m[ 5] *= y; m[ 6] *= y; m[ 7] *= y;
    m[ 8] *= z; m[ 9] *= z; m[10] *= z; m[11] *= z;
}

static m_inline void identity44(float *m) {
    scaling44(m, 1,1,1);
}
static m_inline void copy44(float *m, float *a) {
    for( int i = 0; i < 16; ++i ) m[i] = a[i];
}
static m_inline void multiply44(float *out, const float *a, const float *b) {
    for (int y = 0; y < 4; y++)
        for (int x = 0; x < 4; x++)
            out[y*4 + x] = a[x] * b[y*4] + a[4 + x] * b[y*4 + 1] + a[8 + x] * b[y*4 + 2] + a[12 + x] * b[y*4 + 3];
}
static m_inline void multiply344(float *out, const float *a, const float *b, const float *c) {
    float m[16];
    multiply44(m, a, b);
    multiply44(out, m, c);
}
static m_inline void transpose44(float *m, const float *a) { // M[i][j] = A[j][i];
    m[ 0] = a[0]; m[ 1] = a[4]; m[ 2] = a[ 8]; m[ 3] = a[12];
    m[ 4] = a[1]; m[ 5] = a[5]; m[ 6] = a[ 9]; m[ 7] = a[13];
    m[ 8] = a[2]; m[ 9] = a[6]; m[10] = a[10]; m[11] = a[14];
    m[12] = a[3]; m[13] = a[7]; m[14] = a[11]; m[15] = a[15];
}
static m_inline float det44(const float *M) { // !!! ok, i guess
    float s[6], c[6];
    s[0] = M[0*4+0]*M[1*4+1] - M[1*4+0]*M[0*4+1];
    s[1] = M[0*4+0]*M[1*4+2] - M[1*4+0]*M[0*4+2];
    s[2] = M[0*4+0]*M[1*4+3] - M[1*4+0]*M[0*4+3];
    s[3] = M[0*4+1]*M[1*4+2] - M[1*4+1]*M[0*4+2];
    s[4] = M[0*4+1]*M[1*4+3] - M[1*4+1]*M[0*4+3];
    s[5] = M[0*4+2]*M[1*4+3] - M[1*4+2]*M[0*4+3];

    c[0] = M[2*4+0]*M[3*4+1] - M[3*4+0]*M[2*4+1];
    c[1] = M[2*4+0]*M[3*4+2] - M[3*4+0]*M[2*4+2];
    c[2] = M[2*4+0]*M[3*4+3] - M[3*4+0]*M[2*4+3];
    c[3] = M[2*4+1]*M[3*4+2] - M[3*4+1]*M[2*4+2];
    c[4] = M[2*4+1]*M[3*4+3] - M[3*4+1]*M[2*4+3];
    c[5] = M[2*4+2]*M[3*4+3] - M[3*4+2]*M[2*4+3];
    
    return ( s[0]*c[5]-s[1]*c[4]+s[2]*c[3]+s[3]*c[2]-s[4]*c[1]+s[5]*c[0] );
}
static m_inline void invert44(float *T, const float *M) { // !!! ok, i guess
    float s[6], c[6];
    s[0] = M[0*4+0]*M[1*4+1] - M[1*4+0]*M[0*4+1];
    s[1] = M[0*4+0]*M[1*4+2] - M[1*4+0]*M[0*4+2];
    s[2] = M[0*4+0]*M[1*4+3] - M[1*4+0]*M[0*4+3];
    s[3] = M[0*4+1]*M[1*4+2] - M[1*4+1]*M[0*4+2];
    s[4] = M[0*4+1]*M[1*4+3] - M[1*4+1]*M[0*4+3];
    s[5] = M[0*4+2]*M[1*4+3] - M[1*4+2]*M[0*4+3];

    c[0] = M[2*4+0]*M[3*4+1] - M[3*4+0]*M[2*4+1];
    c[1] = M[2*4+0]*M[3*4+2] - M[3*4+0]*M[2*4+2];
    c[2] = M[2*4+0]*M[3*4+3] - M[3*4+0]*M[2*4+3];
    c[3] = M[2*4+1]*M[3*4+2] - M[3*4+1]*M[2*4+2];
    c[4] = M[2*4+1]*M[3*4+3] - M[3*4+1]*M[2*4+3];
    c[5] = M[2*4+2]*M[3*4+3] - M[3*4+2]*M[2*4+3];
    
    float det = ( s[0]*c[5]-s[1]*c[4]+s[2]*c[3]+s[3]*c[2]-s[4]*c[1]+s[5]*c[0] );
    if( det ) {
        float idet = 1.0f / det;

        T[0*4+0] = ( M[1*4+1] * c[5] - M[1*4+2] * c[4] + M[1*4+3] * c[3]) * idet;
        T[0*4+1] = (-M[0*4+1] * c[5] + M[0*4+2] * c[4] - M[0*4+3] * c[3]) * idet;
        T[0*4+2] = ( M[3*4+1] * s[5] - M[3*4+2] * s[4] + M[3*4+3] * s[3]) * idet;
        T[0*4+3] = (-M[2*4+1] * s[5] + M[2*4+2] * s[4] - M[2*4+3] * s[3]) * idet;

        T[1*4+0] = (-M[1*4+0] * c[5] + M[1*4+2] * c[2] - M[1*4+3] * c[1]) * idet;
        T[1*4+1] = ( M[0*4+0] * c[5] - M[0*4+2] * c[2] + M[0*4+3] * c[1]) * idet;
        T[1*4+2] = (-M[3*4+0] * s[5] + M[3*4+2] * s[2] - M[3*4+3] * s[1]) * idet;
        T[1*4+3] = ( M[2*4+0] * s[5] - M[2*4+2] * s[2] + M[2*4+3] * s[1]) * idet;

        T[2*4+0] = ( M[1*4+0] * c[4] - M[1*4+1] * c[2] + M[1*4+3] * c[0]) * idet;
        T[2*4+1] = (-M[0*4+0] * c[4] + M[0*4+1] * c[2] - M[0*4+3] * c[0]) * idet;
        T[2*4+2] = ( M[3*4+0] * s[4] - M[3*4+1] * s[2] + M[3*4+3] * s[0]) * idet;
        T[2*4+3] = (-M[2*4+0] * s[4] + M[2*4+1] * s[2] - M[2*4+3] * s[0]) * idet;

        T[3*4+0] = (-M[1*4+0] * c[3] + M[1*4+1] * c[1] - M[1*4+2] * c[0]) * idet;
        T[3*4+1] = ( M[0*4+0] * c[3] - M[0*4+1] * c[1] + M[0*4+2] * c[0]) * idet;
        T[3*4+2] = (-M[3*4+0] * s[3] + M[3*4+1] * s[1] - M[3*4+2] * s[0]) * idet;
        T[3*4+3] = ( M[2*4+0] * s[3] - M[2*4+1] * s[1] + M[2*4+2] * s[0]) * idet;
    }
}
static m_inline vec3 transform44(const float *m, const vec3 p) {
    float d = 1.0f / (m[3] * p.x + m[7] * p.y + m[11] * p.z + m[15]);
    return vec3(
        d * (m[0] * p.x + m[4] * p.y + m[ 8] * p.z + m[12]),
        d * (m[1] * p.x + m[5] * p.y + m[ 9] * p.z + m[13]),
        d * (m[2] * p.x + m[6] * p.y + m[10] * p.z + m[14])
    );
}




// !!! for debugging
#include <stdio.h>
static void print44( float *m ) {
    for(int i = 0; i < 4; ++i)
        printf("%5.2f, %5.2f, %5.2f, %5.2f\n", m[i*4+0], m[i*4+1], m[i*4+2], m[i*4+3]);
    puts("---");
}
