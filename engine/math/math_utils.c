// includes code by @glampert (public domain).

#pragma once
#include "../detect/detect_inline.c"
#include "math_linear.c"
#include <float.h> // FLT_MIN, FLT_MAX
#include <stdint.h>
#include <math.h>

#ifndef M_PI // even with _USE_MATH_DEFINES not always available
#define M_PI 3.14159265358979323846
#endif

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

static m_inline float floatRound(float x) {
    // Probably slower than std::floor(), also depends of FPU settings,
    // but we only need this for that special sin/cos() case anyways...
    const int i = (int)(x);
    return (x >= 0.0f) ? (float)(i) : (float)(i - 1);
}

static m_inline float floatAbs(float x) {
    // Mask-off the sign bit
    Float2UInt i;
    i.asFloat = x;
    i.asUInt &= 0x7FFFFFFF;
    return i.asFloat;
}

static m_inline float floatInvSqrt(float x) {
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

static m_inline float floatSin(float radians) {
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

static m_inline float floatCos(float radians) {
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
