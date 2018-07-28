// includes code by @glampert (public domain).

#pragma once

#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif

#include <math.h>
#include <float.h> // FLT_MIN, FLT_MAX

#ifndef M_PI // even with _USE_MATH_DEFINES not always available
#define M_PI 3.14159265358979323846
#endif

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

    q[0] = cr * sp * cy - sr * cp * sy;
    q[1] = cr * cp * sy + sr * sp * cy;
    q[2] = sr * cp * cy - cr * sp * sy;
    q[3] = cr * cp * cy + sr * sp * sy;
}


/*
#include "math/matrix.h"
#include "math/vector.h"
#include "math/quaternion.h"
*/

#define vec2set(p, x,y)     (p[0] = x, p[1] = y, p)
#define vec3set(p, x,y,z)   (p[0] = x, p[1] = y, p[2] = z, p)
#define vec4set(p, x,y,z,w) (p[0] = x, p[1] = y, p[2] = z, p[3] = w, p)
#define vec2cpy(p, q)       vec2set(p, q[0], q[1])
#define vec3cpy(p, q)       vec3set(p, q[0], q[1], q[2])
#define vec4cpy(p, q)       vec4set(p, q[0], q[1], q[2], q[3])

#define vec3add(p, v)   (p[0] += v[0], p[1] += v[1], p[2] += v[2], p)
#define vec3sub(p, v)   (p[0] -= v[0], p[1] -= v[1], p[2] -= v[2], p)
#define vec3mul(p, v)   (p[0] *= v[0], p[1] *= v[1], p[2] *= v[2], p)
#define vec3div(p, v)   (p[0] /= v[0], p[1] /= v[1], p[2] /= v[2], p)

#define vec3inc(p, f)   (p[0] += f, p[1] += f, p[2] += f, p)
#define vec3dec(p, f)   (p[0] -= f, p[1] -= f, p[2] -= f, p)
#define vec3scale(p, f) (p[0] *= f, p[1] *= f, p[2] *= f, p)

// ----------------------------------------------------------------------------
// Fast approximations of math functions.
// - by @glampert, public domain.

#include <float.h>
static const float FloatEpsilon = FLT_EPSILON;
static const float PI = M_PI;
static const float HalfPI = M_PI * 0.5f;
static const float TAU    = M_PI * 2.0f;

typedef union Float2UInt {
    float asFloat;
    uint32_t asUInt;
} Float2UInt;

static inline float floatRound(float x) {
    // Probably slower than std::floor(), also depends of FPU settings,
    // but we only need this for that special sin/cos() case anyways...
    const int i = (int)(x);
    return (x >= 0.0f) ? (float)(i) : (float)(i - 1);
}

static inline float floatAbs(float x) {
    // Mask-off the sign bit
    Float2UInt i;
    i.asFloat = x;
    i.asUInt &= 0x7FFFFFFF;
    return i.asFloat;
}

static inline float floatInvSqrt(float x) {
    // Modified version of the emblematic Q_rsqrt() from Quake 3.
    // See: http://en.wikipedia.org/wiki/Fast_inverse_square_root
    Float2UInt i;
    float y, r;
    y = x * 0.5f;
    i.asFloat = x;
    i.asUInt = 0x5F3759DF - (i.asUInt >> 1);
    r = i.asFloat;
    r = r * (1.5f - (r * r * y));
    return r;
}

static inline float floatSin(float radians) {
    static const float A = -2.39e-08;
    static const float B = 2.7526e-06;
    static const float C = 1.98409e-04;
    static const float D = 8.3333315e-03;
    static const float E = 1.666666664e-01;

    if (radians < 0.0f || radians >= TAU)
    {
        radians -= floatRound(radians / TAU) * TAU;
    }

    if (radians < PI)
    {
        if (radians > HalfPI)
        {
            radians = PI - radians;
        }
    }
    else
    {
        radians = (radians > (PI + HalfPI)) ? (radians - TAU) : (PI - radians);
    }

    const float s = radians * radians;
    return radians * (((((A * s + B) * s - C) * s + D) * s - E) * s + 1.0f);
}

static inline float floatCos(float radians) {
    static const float A = -2.605e-07;
    static const float B = 2.47609e-05;
    static const float C = 1.3888397e-03;
    static const float D = 4.16666418e-02;
    static const float E = 4.999999963e-01;

    if (radians < 0.0f || radians >= TAU) {
        radians -= floatRound(radians / TAU) * TAU;
    }

    float d;
    if (radians < PI) {
        if (radians > HalfPI)
        {
            radians = PI - radians;
            d = -1.0f;
        }
        else
        {
            d = 1.0f;
        }
    } else {
        if (radians > (PI + HalfPI))
        {
            radians = radians - TAU;
            d = 1.0f;
        }
        else
        {
            radians = PI - radians;
            d = -1.0f;
        }
    }

    const float s = radians * radians;
    return d * (((((A * s + B) * s - C) * s + D) * s - E) * s + 1.0f);
}
